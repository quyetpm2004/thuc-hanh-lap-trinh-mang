#include <stdio.h>
#include <poll.h>

int main()
{
    int sockfd1 = 1;
    int sockfd2 = 2;
    struct pollfd fds[2];
    fds[0].fd = sockfd1;
    fds[0].events = POLLIN;
    fds[1].fd = sockfd2;
    fds[1].events = POLLIN;

    int ret = poll(fds, 2, 10000); // 10 giây timeout
    if (ret == -1)
    {
        perror("poll error");
    }
    else if (ret == 0)
    {
        printf("Timeout, không có dữ liệu\n");
    }
    else
    {
        for (int i = 0; i < 2; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                printf("Socket %d có dữ liệu\n", fds[i].fd);
            }
        }
    }

    return 0;
}
