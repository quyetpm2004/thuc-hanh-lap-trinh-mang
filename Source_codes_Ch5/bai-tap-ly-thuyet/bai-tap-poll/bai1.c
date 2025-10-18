#include <stdio.h>
#include <poll.h>

int main()
{
    struct pollfd fds[1];
    fds[0].fd = 0;
    fds[0].events = POLLIN;

    return 0;
}
