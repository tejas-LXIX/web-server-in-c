#ifndef _COMMON_H_
#define _COMMON_H_
#endif

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

#define SERVER_PORT 18000

#define MAXLINE 4096
#define SA struct sockaddr

// prints out the error to the console.
void err_n_die(const char* fmt, ...);

// only used for debugging purposes. converts binary to hex data.
char* bin2hex(const unsigned char* input, size_t len);