#include "common.h"
#include<strings.h>

int main(int argc, char** argv) {
    //listen file descriptor, connection file descriptor
    int listenfd, connfd, n;
    struct sockaddr_in servaddr;

    //response buffer
    uint8_t buff[MAXLINE+1];

    //request buffer
    uint8_t recvline[MAXLINE+1];

    //create a socket and assign it's file descriptor to listenfd
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err_n_die("socket error.");
    }

    //zero out the servaddr struct before setting values in it.
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   // means that the socket will be bound to all local network interfaces (wireless, ethernet, localhost etc.).
    servaddr.sin_port = htons(SERVER_PORT);

    //bind the socket to the specified address.
    if ((bind(listenfd, (SA *) &servaddr, sizeof(servaddr))) < 0) {
        err_n_die("bind error.");
    }

    //start listening on the socket. listen() marks the socket referred to by listenfd as a passive socket, that is, as a socket that will be used to accept incoming connection requests using accept().https://man7.org/linux/man-pages/man2/listen.2.html
    if ((listen(listenfd, 10)) < 0) {
        err_n_die("listen error.");
    }

    //endless loop
    for ( ; ; ) {
        struct sockaddr_in addr;
        socklen_t addr_len;

        printf("waiting for a connection on port %d\n",SERVER_PORT);
        fflush(stdout);

        //accept the connection. The accept() system call is used with connection-based socket types (SOCK_STREAM, SOCK_SEQPACKET). It extracts the first connection request on the queue of pending connections for the
        //listening socket, listenfd, creates a new connected socket, and returns a new file descriptor referring to that socket.  The newly created socket is not in the listening state.  The original socket listenfd is unaffected by this call.
        //https://man7.org/linux/man-pages/man2/accept.2.html
        //accept() is a blocking operation. it waits till someone connects.
        connfd = accept(listenfd, (SA *) NULL, NULL);

        // zero out the receive buffer before reading anything.
        memset(recvline, 0, MAXLINE);

        //https://man7.org/linux/man-pages/man2/read.2.html
        if ( (n = read(connfd, recvline, MAXLINE-1)) > 0) {
            fprintf(stdout, "\n%s\n\n%s", bin2hex(recvline, n), recvline);
        }
        if (n < 0) {
            err_n_die("read error");
        }

        snprintf((char*) buff, sizeof(buff), "HTTP/1.0 200 OK \r\n\r\nHello from Tejas");

        //https://man7.org/linux/man-pages/man2/write.2.html
        write(connfd, (char*) buff, strlen((char *)buff));

        //https://pubs.opengroup.org/onlinepubs/009604499/functions/close.html
        close(connfd);
    }
}