#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>

int main()
{
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Initialize exceptfds set
    fd_set exceptfds;
    FD_ZERO(&exceptfds);
    FD_SET(sockfd, &exceptfds);

    // Set timeout to 5 seconds
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    printf("Waiting for exceptional conditions on socket (5 sec)...\n");

    // Wait for any exceptional events (like OOB data or socket errors)
    int result = select(sockfd + 1, NULL, NULL, &exceptfds, &timeout);

    if (result < 0)
    {
        perror("select");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    else if (result == 0)
    {
        printf("No exceptional condition detected (timeout reached).\n");
    }
    else
    {
        if (FD_ISSET(sockfd, &exceptfds))
        {
            printf("Exceptional condition detected on socket fd=%d!\n", sockfd);
        }
    }

    close(sockfd);
    return 0;
}
