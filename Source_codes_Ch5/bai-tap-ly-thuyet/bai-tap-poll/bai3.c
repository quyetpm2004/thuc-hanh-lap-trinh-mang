#include <stdio.h>
#include <poll.h>

int main()
{
    int sockfd1 = 3;
    int sockfd2 = 4;
    struct pollfd fds[2];
    fds[0].fd = sockfd1;
    fds[0].events = POLLIN;
    fds[1].fd = sockfd2;
    fds[1].events = POLLIN;
    poll(fds, 2, 10000); // 10 giây timeout

    return 0;
}
