#include <stdio.h>
#include <poll.h>

int main()
{
    struct pollfd fds[1];
    fds[0].fd = 0;
    fds[0].events = POLLIN;

    printf("Đang theo dõi stdin (nhập dữ liệu để kiểm tra)...\n");

    int ret = poll(fds, 1, 0); // 5 giây timeout
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
        if (fds[0].revents & POLLIN)
        {
            printf("Có dữ liệu từ stdin\n");
        }
    }

    return 0;
}
