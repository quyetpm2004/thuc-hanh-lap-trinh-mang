#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <sys/socket.h>

int main()
{
    int sv[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);

    struct pollfd fds[1];
    fds[0].fd = sv[0];
    fds[0].events = POLLIN;

    close(sv[1]);

    int ret = poll(fds, 1, 5000);
    if (ret > 0)
    {
        if (fds[0].revents & POLLHUP)
            printf("Client đóng kết nối\n");
        else if (fds[0].revents & POLLIN)
            printf("Có dữ liệu để đọc\n");
    }
    else if (ret == 0)
    {
        printf("Hết thời gian chờ\n");
    }
    else
    {
        printf("Lỗi khi gọi poll()\n");
    }

    close(sv[0]);
    return 0;
}
