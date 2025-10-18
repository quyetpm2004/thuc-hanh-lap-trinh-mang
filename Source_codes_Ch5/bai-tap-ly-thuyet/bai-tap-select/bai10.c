#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>

int main()
{
    int sockfd1, sockfd2;
    sockfd1 = socket(AF_INET, SOCK_STREAM, 0);
    sockfd2 = socket(AF_INET, SOCK_STREAM, 0);

    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(sockfd1, &read_fds);
    FD_SET(sockfd2, &read_fds);
    int maxfd = (sockfd1 > sockfd2) ? sockfd1 : sockfd2;

    int result = select(maxfd + 1, &read_fds, NULL, NULL, NULL);
    if (result > 0)
    {
        if (FD_ISSET(sockfd1, &read_fds))
        {
            printf("Socket 1 co gia tri %d san sang de doc!\n", sockfd1);
        }
        if (FD_ISSET(sockfd2, &read_fds))
        {
            printf("Socket 2 co gia tri %d san sang de doc!\n", sockfd2);
        }
    }
    else if (result == 0)
    {
        printf("Khong co Socket nao san sang\n");
    }
    else
    {
        printf("Co loi xay ra\n");
    }

    return 0;
}