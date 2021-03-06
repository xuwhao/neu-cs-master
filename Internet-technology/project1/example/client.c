#include "stdio.h"
#include <windows.h>
#include <winsock.h>
#define PROTOPORT 6666          /* default protocol port number */
char localhost[] = "localhost"; /* default host name            */
int main(int argc, char *argv[]) {
    struct hostent *ptrh;   /* pointer to a host table entry       */
    struct sockaddr_in sad; /* structure to hold an IP address     */
    int sd;                 /* socket descriptor                   */
    int port;               /* protocol port number                */
    char *host;             /* pointer to host name                */
    int n;                  /* number of characters read           */
    char buf[1000];         /* buffer for data from the server     */
    WSADATA wsaData;
    WSAStartup(0x0101, &wsaData);
    memset((char *)&sad, 0, sizeof(sad)); /* clear sockaddr structure   */
    sad.sin_family = AF_INET;             /* set family to Internet     */
    /* Check command-line argument for protocol port and extract      */
    /* port number if one is specified.  Otherwise, use the default   */
    /* port value given by constant PROTOPORT                         */
    if (argc > 2)             /* if protocol port specified     */
        port = atoi(argv[2]); /* convert to binary              */
    else
        port = PROTOPORT; /* use default port number        */
    if (port > 0)         /* test for legal value           */
        sad.sin_port = htons((u_short)port);
    else { /* print error message and exit    */
        fprintf(stderr, "bad port number %s\n", argv[2]);
        exit(1);
    }
    /* Check host argument and assign host name. */

    if (argc > 1)
        host = argv[1]; /* if host argument specified     */
    else
        host = localhost;
    /* Convert host name to equivalent IP address and copy to sad.    */
    ptrh = gethostbyname(host);
    if (((char *)ptrh) == NULL) {
        fprintf(stderr, "invalid host: %s\n", host);
        exit(1);
    }
    memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);
    /* Create a socket. */
    sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0) {
        fprintf(stderr, "socket creation failed\n");
        exit(1);
    }
    /* Connect the socket to the specified server. */
    if (connect(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
        fprintf(stderr, "connect failed\n");
        exit(1);
    }
    /* Repeatedly read data from socket and write to user's screen.  */
    n = recv(sd, buf, sizeof(buf), 0);
    while (n > 0) {
        // write(buf,1,n);
        buf[n] = 0;
        printf("%s", buf);
        n = recv(sd, buf, sizeof(buf), 0);
    }
    closesocket(sd); /* Close the socket. */
    exit(0);         /* Terminate the client program gracefully. */
}
