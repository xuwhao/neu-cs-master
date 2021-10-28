#include "stdio.h"
#include <winsock.h>
#pragma comment(lib, "ws2_32.lib")
#define PROTOPORT 6666 /* default protocol port number */
#define QLEN 6         /* size of request queue        */
int visits = 0;        /* counts client connections    */
int main(int argc, char *argv[]) {
    struct sockaddr_in sad; /* structure to hold server's address  */
    struct sockaddr_in cad; /* structure to hold client's address  */
    int sd, sd2;            /* socket descriptors                  */
    int port;               /* protocol port number                */
    int alen;               /* length of address                   */
    char buf[1000];         /* buffer for string the server sends  */
    WSADATA wsaData;
    WSAStartup(0x0101, &wsaData);
    memset((char *)&sad, 0, sizeof(sad)); /* clear sockaddr structure */
    sad.sin_family = AF_INET;             /* set family to Internet   */
    sad.sin_addr.s_addr = INADDR_ANY;     /* set the local IP address */
    /* Check command-line argument for protocol port and extract     */
    /* port number if one is specified.  Otherwise, use the default  */
    /* port value given by constant PROTOPORT                        */
    if (argc > 1)             /* if argument specified          */
        port = atoi(argv[1]); /* convert argument to binary    */
    else
        port = PROTOPORT; /* use default port number       */
    if (port > 0)         /* test for illegal value        */
        sad.sin_port = htons((u_short)port);
    else { /* print error message and exit  */
        fprintf(stderr, "bad port number %s\n", argv[1]);
        exit(1);
    }
    /* Create a socket */
    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0) {
        fprintf(stderr, "socket creation failed\n");
        exit(1);
    }
    /* Bind a local address to the socket */
    if (bind(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
        fprintf(stderr, "bind failed\n");
        exit(1);
    }
    /* Specify size of request queue */
    if (listen(sd, QLEN) < 0) {
        fprintf(stderr, "listen failed\n");
        exit(1);
    }
    /* Main server loop - accept and handle requests */
    while (1) {
        alen = sizeof(cad);
        if ((sd2 = accept(sd, (struct sockaddr *)&cad, &alen)) < 0) {
            fprintf(stderr, "accept failed\n");
            exit(1);
        }
        visits++;
        sprintf(buf, "本服务器已被连接了%d次\n", visits);
        send(sd2, buf, strlen(buf), 0);
        printf("%s", buf);
        closesocket(sd2);
    }
}
