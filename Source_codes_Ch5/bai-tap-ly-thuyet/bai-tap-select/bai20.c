#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>

int my_select(int maxfd, fd_set *readfds, struct timeval *timeout)
{
    return select(maxfd + 1, readfds, NULL, NULL, timeout);
}

int main()
{
    fd_set readfds;
    struct timeval timeout;

    FD_ZERO(&readfds);
    FD_SET(0, &readfds);

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    printf("Chờ nhập từ bàn phím (5 giây)...\n");

    int result = my_select(0, &readfds, &timeout);

    if (result < 0)
        perror("Lỗi khi gọi select");
    else if (result == 0)
        printf("Hết thời gian chờ, không có dữ liệu nhập.\n");
    else if (FD_ISSET(0, &readfds))
        printf("Phát hiện dữ liệu từ bàn phím!\n");

    return 0;
}
