#include <stdio.h>
#include <winsock.h>
#pragma comment(lib, "ws2_32.lib")

#define PROTOPORT 6666
#define MSG_LEN 1000
int visits = 0;

int main(int argc, char *argv[]) {
    WSADATA wsaData;
    int err = WSAStartup(0x0101, &wsaData);
    if (err != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", err);
        return 1;
    }

    int port = PROTOPORT;
    char *ip = "127.0.0.1";
    if (argc == 3) {
        ip = argv[1];
        port = atoi(argv[2]);
    }

    // setup an UDP socket
    SOCKET socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_fd == INVALID_SOCKET) {
        fprintf(stderr, "create socket with error: %d\n", WSAGetLastError());
        return 2;
    }

    SOCKADDR_IN server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    // send a message to the server
    int server_len = sizeof(server_addr);
    char *msg = malloc(sizeof(char) * MSG_LEN);
    strcpy(msg, "Hello World!");
    err = sendto(socket_fd, msg, strlen(msg), 0, (SOCKADDR *)&server_addr,
                 server_len);
    if (err == SOCKET_ERROR) {
        fprintf(stderr, "sendto with error: %d\n", err);
        return 3;
    }

    // receive the response
    err = recvfrom(socket_fd, msg, MSG_LEN, 0, (SOCKADDR *)&server_addr,
                   &server_len);
    if (err < 0) {
        fprintf(stderr, "recvfrom with error: %d\n", err);
        return 4;
    }

    closesocket(socket_fd);
    WSACleanup();
    msg[err] = 0;
    fprintf(stdout, msg);
}
