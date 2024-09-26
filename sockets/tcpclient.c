#include<sys/socket.h>  //basic socket definitions
#include<sys/types.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<stdarg.h>  //for variadic function
#include<errno.h>
#include<fcntl.h>
#include<sys/time.h>
#include<sys/ioctl.h>
#include<netdb.h>

/*
Socket is nothing but a file in UNIX operating system. Even everything is treated as a file in UNIX Operating system.
Whenever we create a socket an entry is made in the file descriptor table which contains standard i/o and standard errors and other details.
The file descriptor acts as a pointer to the File Table which contains information about what action is to be taken i.e read, write, etc, and it contains pointers to the inode table of that particular file and as you might know inode contains all the necessary details of a file.
*/

#define SERVER_PORT 80

#define MAXLINE 4096    //size of the buffer to read the response.
#define SA struct sockaddr  //The sockaddr structure is a generic structure that specifies a transport address. https://learn.microsoft.com/en-us/windows/win32/api/ws2def/ns-ws2def-sockaddr
/*
Members of AF_INET address family are IPv4 addresses.
Members of AF_INET6 address family are IPv6 addresses.
Members of AF_UNIX address family are names of Unix domain sockets (/var/run/mysqld/mysqld.sock is an example).
Members of AF_IPX address family are IPX addresses, and so on.
*/

void err_and_die(const char *fmt, ...);

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

// In C, the ellipsis (...) is used in function signatures to indicate that the function can accept an arbitrary number of arguments.
// https://hackernoon.com/what-is-va_list-in-c-exploring-the-secrets-of-ft_printf to understand va_list, va_start, va_end etc.
void err_and_die(const char *fmt, ...) {
    int errno_save;
    va_list ap; //At its core, va_list is a way for C functions to accept a variable number of arguments. It's like a special kind of list that holds all the extra arguments you pass to a function.

/* The <errno.h> header file defines the integer variable errno,
         which is set by system calls and some library functions in the
         event of an error to indicate what went wrong.
We save it in a local variable immediately so as to not lose it.
*/
    errno_save = errno; //https://man7.org/linux/man-pages/man3/errno.3.html

    va_start(ap,fmt);   //Think of this as "starting the list". It initialises the list to point to the first variable argument
    vfprintf(stdout, fmt, ap);  //The C library function int vfprintf(FILE *stream, const char *format, va_list arg) sends formatted output to a stream using an argument list passed to it.
    fprintf(stdout, "\n");  // The C library fprintf() function is used to write formatted data to a stream. It is part of the standard I/O library <stdio.h> and allows you to write data to a file stream as opposed to printf() which writes to the standard output stream.
    fflush(stdout); // The C library fflush() function flushes the output buffer of a stream.This function forces a write of all buffered data for the given output or update stream to the file. When applied to an input stream, its behavior is undefined. Flushing the stream ensures that any data buffered in memory is written out to the file or device associated with the stream.

//print system call/library error also if errno was set.
    if (errno_save != 0) {
        fprintf(stdout, "(errno = %d) : %s\n", errno_save, strerror(errno_save));
        fprintf(stdout, "\n");
        fflush(stdout);
    }
    va_end(ap); //This "ends the list" and cleans things up.

    exit(1);
}