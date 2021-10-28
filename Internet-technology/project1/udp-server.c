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
    if (argc > 1) {
        port = atoi(argv[1]);
    }

    // setup an UDP socket
    SOCKET socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_fd == INVALID_SOCKET) {
        fprintf(stderr, "create socket with error: %d\n", WSAGetLastError());
        return 2;
    }

    SOCKADDR_IN server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // bind the fd to a specific address
    err = bind(socket_fd, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));
    if (err != 0) {
        fprintf(stderr, "bind with error: %d\n", WSAGetLastError());
        return 3;
    }

    // Keep listening the data
    int client_len = sizeof(client_addr);
    char msg[MSG_LEN];
    while (1) {
        err = recvfrom(socket_fd, msg, MSG_LEN, 0, (SOCKADDR *)&client_addr, &client_len);
        if (err < 0) {
            fprintf(stderr, "recvfrom with error: %d\n", err);
            return 4;
        }

        sprintf(msg, "服务器已被连接 %d 次\n", ++visits);
        fprintf(stdout, msg);

        err = sendto(socket_fd, msg, strlen(msg), 0, (SOCKADDR *)&client_addr, client_len);
        if (err == SOCKET_ERROR) {
            fprintf(stderr, "sendto with error: %d\n", err);
            return 5;
        }
    }
}
