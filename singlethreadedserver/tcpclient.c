#include "common.h"

int main(int argc, char **argv) {
    int sockfd, n;
    int sendbytes;
    struct sockaddr_in servaddr;    //The SOCKADDR_IN structure specifies a transport address and port for the AF_INET address family. https://learn.microsoft.com/en-us/windows/win32/api/ws2def/ns-ws2def-sockaddr_in
    char sendline[MAXLINE];
    char recvline[MAXLINE];

    if (argc != 2) {
        err_and_die("usage: %s <server address>", argv[0]);
    }

    // create a new socket. https://pubs.opengroup.org/onlinepubs/007904975/functions/socket.html
    // TCP (SOCK_STREAM) is a connection-based protocol. The connection is established and the two parties have a conversation until the connection is terminated by one of the parties or by a network error.
    //https://www.ibm.com/docs/en/aix/7.1?topic=protocols-socket-types
    // Upon successful completion, socket() shall return a non-negative integer, the socket file descriptor. Otherwise, a value of -1 shall be returned and errno set to indicate the error.
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err_and_die("Error while creating the socket!");
    }

    //https://pubs.opengroup.org/onlinepubs/007904975/functions/bzero.html.
    //The bzero() function shall place n zero-valued bytes in the area pointed to by s. similar to memset, but memset is better.
    //zero out the address.
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;  //specify the address family

    //The htons() (host to network short) function converts the unsigned short integer parameter from host byte order to network byte order. EG: Big Endian to network standard byte order.
    //This ensures that two computers that use different byte orders can still communicate with each other.
    servaddr.sin_port = htons(SERVER_PORT); //specify the port.

    //This inet_pton function converts the character string src into a network address structure in the af address family, then copies the
    //network address structure to dst.  The af argument must be either AF_INET or AF_INET6.  dst is written in network byte order.
    //Basically, it converts the string representation of the IP address into a binary representation.
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        err_and_die("inet_pton error for %s ", argv[1]);
    }

    //https://pubs.opengroup.org/onlinepubs/007904975/functions/connect.html
    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0) {
        err_and_die("connect failed");
    }

    //The C Library sprintf() function allows you to create strings with specified formats, similar to printf(), but instead of printing to the standard output, it stores the resulting string in a character array provided by the user.
    sprintf(sendline, "GET / HTTP/1.1\r\n\r\n");
    sendbytes = strlen(sendline);

    //https://pubs.opengroup.org/onlinepubs/009696699/functions/write.html
    //The write() function shall attempt to write nbyte bytes from the buffer pointed to by buf to the file associated with the open file descriptor, fildes.
    //write data into the socket. it will send these characters over the network to the server.
    if (write(sockfd, sendline, sendbytes) != sendbytes) {
        err_and_die("write error");
    }

    //The read() function shall attempt to read nbyte bytes from the file associated with the open file descriptor, fildes, into the buffer pointed to by buf. The behavior of multiple concurrent reads on the same pipe, FIFO, or terminal device is unspecified.
    //maxline-1 so that the last byte can be the null terminated character
    while ( (n=read(sockfd, recvline, MAXLINE-1)) > 0) {
        printf("%s", recvline);
        //zero out the recvline array every time. It makes sure that the string (character array) will be null terminated when response is put into the buffer.
        memset(recvline, 0, MAXLINE);
    }

    if (n < 0) {
        err_and_die("read error");
    }

    fprintf(stdout, "\nEXITING THE PROGRAM\n");

    exit(0);
}