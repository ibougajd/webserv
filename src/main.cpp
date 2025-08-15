#include <iostream>
#include <sys/socket.h>
#include <cstdio>









int main()
{
    int server_fd;
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Cannot create socket");
        return(1);
    }
}