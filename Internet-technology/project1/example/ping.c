#include "stdio.h"
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define WIN32_LEAN_AND_MEAN
#define ICMP_ECHO 8      // 请求回显 ping请求 请求回显8
#define ICMP_ECHOREPLY 0 // 回显应答0
#define ICMP_MIN 12      // minimum 12 byte icmp message(just header)
// The IP header  20bytes
typedef struct iphdr {
    unsigned char h_len : 4;       // length of the header
    unsigned char version : 4;     // Version of IP
    unsigned char tos;             // Type of service
    unsigned short total_len;      // total length of the packet
    unsigned short ident;          // unique identifier
    unsigned short frag_and_flags; // flags
    unsigned char ttl;
    unsigned char proto; // protocol (TCP, UDP etc)
    unsigned int sourceIP;
    unsigned int destIP;
} IpHeader;
// ICMP header  12 bytes
typedef struct _ihdr {
    BYTE i_type;
    BYTE i_code; // type sub code
    USHORT i_cksum;
    USHORT i_id;
    USHORT i_seq;
    ULONG timestamp; // This is not the std header. It's reserved for time.
} IcmpHeader;
#define STATUS_FAILED 0xFFFF
#define MAX_PACKET 1024
#define xmalloc(s) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (s))
#define xfree(p) HeapFree(GetProcessHeap(), 0, (p))
USHORT checksum(USHORT *, int);
void fill_icmp_head(char *);
void decode_resp(char *, int, struct sockaddr_in *);
void Usage(char *progname) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "%s <host>\n", progname);
    ExitProcess(STATUS_FAILED);
}
int main(int argc, char **argv) {
    WSADATA wsaData;
    SOCKET sockRaw;
    struct sockaddr_in dest, from;
    struct hostent *hp;
    int bread, datasize;
    int fromlen = sizeof(from);
    char *dest_ip;
    char *icmp_data;
    char *recvbuf;
    unsigned int addr = 0;
    USHORT seq_no = 0;
    if (WSAStartup(0x0101, &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", GetLastError());
        ExitProcess(STATUS_FAILED);
    }
    if (argc < 2) {
        argc = 2;
        argv[1] = "localhost";
    }
    if ((sockRaw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == INVALID_SOCKET) {
        fprintf(stderr, "WSAStartup failed: %d\n", GetLastError());
        ExitProcess(STATUS_FAILED);
    }
    memset(&dest, 0, sizeof(dest));
    hp = gethostbyname(argv[1]);
    if (hp != NULL) {
        memcpy(&(dest.sin_addr), hp->h_addr, hp->h_length);
        dest.sin_family = AF_INET;
        dest_ip = inet_ntoa(dest.sin_addr);
    } else {
        fprintf(stderr, "Unable to resolve %s\n", argv[1]);
        ExitProcess(STATUS_FAILED);
    }
    datasize = sizeof(IcmpHeader);           // 12
    icmp_data = (char *)xmalloc(MAX_PACKET); // 1024
    recvbuf = (char *)xmalloc(MAX_PACKET);   // 1024
    if (!icmp_data) {
        fprintf(stderr, "HeapAlloc failed %d\n", GetLastError());
        ExitProcess(STATUS_FAILED);
    }
    memset(icmp_data, 0, MAX_PACKET);
    fill_icmp_head(icmp_data);
    while (1) {
        int bwrote;
        ((IcmpHeader *)icmp_data)->i_cksum = 0;
        ((IcmpHeader *)icmp_data)->timestamp = GetTickCount();
        ((IcmpHeader *)icmp_data)->i_seq = seq_no++;
        ((IcmpHeader *)icmp_data)->i_cksum =
            checksum((USHORT *)icmp_data, sizeof(IcmpHeader));
        bwrote = sendto(sockRaw, icmp_data, datasize, 0,
                        (struct sockaddr *)&dest, sizeof(dest));
        if (bwrote == SOCKET_ERROR) {
            fprintf(stderr, "sendto failed: %d\n", WSAGetLastError());
            ExitProcess(STATUS_FAILED);
        }
        if (bwrote < datasize) {
            fprintf(stdout, "Wrote %d bytes\n", bwrote);
        }
        bread = recvfrom(sockRaw, recvbuf, MAX_PACKET, 0,
                         (struct sockaddr *)&from, &fromlen);
        if (bread == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAETIMEDOUT) {
                printf("timed out\n");
                continue;
            }
            fprintf(stderr, "recvfrom failed: %d\n", WSAGetLastError());
            ExitProcess(STATUS_FAILED);
        }
        decode_resp(recvbuf, bread, &from);
        Sleep(1000); // Windows下 S要大写
    }
    closesocket(sockRaw);
    xfree(icmp_data);
    xfree(recvbuf);
    WSACleanup(); // clean up ws2_32.dll
    return 0;
}
void fill_icmp_head(char *icmp_data) {
    IcmpHeader *icmp_hdr;
    icmp_hdr = (IcmpHeader *)icmp_data;
    icmp_hdr->i_type = ICMP_ECHO;
    icmp_hdr->i_code = 0;
    icmp_hdr->i_cksum = 0;
    icmp_hdr->i_id = (USHORT)GetCurrentProcessId();
    icmp_hdr->i_seq = 0;
}
// The response is an IP packet. We need decode the IP header to locate the ICMP
// data
void decode_resp(char *buf, int bytes, struct sockaddr_in *from) {
    IpHeader *iphdr;
    IcmpHeader *icmphdr;
    unsigned short iphdrlen;
    iphdr = (IpHeader *)buf;
    iphdrlen = iphdr->h_len * 4; // number of 32-bit words *4 = bytes
    if (bytes < iphdrlen + ICMP_MIN) {
        printf("Too few bytes from %s\n", inet_ntoa(from->sin_addr));
    }
    icmphdr = (IcmpHeader *)(buf + iphdrlen);
    if (icmphdr->i_type != ICMP_ECHOREPLY) {
        fprintf(stderr, "non-echo type %d recvd\n", icmphdr->i_type);
        return;
    }
    if (icmphdr->i_id != (USHORT)GetCurrentProcessId()) {
        fprintf(stderr, "someone else's packet!\n");
        return;
    }
    printf("%d bytes from %s:", bytes, inet_ntoa(from->sin_addr));
    printf(" icmp_seq = %d. ", icmphdr->i_seq);
    printf(" time: %d ms ", GetTickCount() - icmphdr->timestamp);
    printf("\n");
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