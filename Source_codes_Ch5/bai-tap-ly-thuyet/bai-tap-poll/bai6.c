#include <stdio.h>
#include <poll.h>

int main()
{
    struct pollfd fds[1];
    fds[0].fd = -1;
    fds[0].events = POLLIN;

    int ret = poll(fds, 1, 1000);
    if (ret > 0)
    {
        if (fds[0].revents & POLLNVAL)
        {
            printf("FD không hợp lệ\n");
        }
    }
    else if (ret == 0)
    {
        printf("Hết thời gian chờ\n");
    }
    else
    {
        printf("Lỗi khi gọi poll()\n");
    }

    return 0;
}
