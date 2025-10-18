#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <errno.h>

int main()
{
    int sockets[FD_SETSIZE];
    fd_set read_fds;
    int i;

    for (i = 0; i < FD_SETSIZE; i++)
    {
        sockets[i] = socket(AF_INET, SOCK_STREAM, 0);
        if (sockets[i] < 0)
        {
            perror("Lỗi tạo socket");
            printf("Dừng lại ở socket #%d (đã đạt giới hạn)\n", i);
            break;
        }
    }

    printf("Tạo thành công %d socket (FD_SETSIZE = %d)\n", i, FD_SETSIZE);

    FD_ZERO(&read_fds);
    int maxfd = 0;

    for (int j = 0; j < i; j++)
    {
        FD_SET(sockets[j], &read_fds);
        if (sockets[j] > maxfd)
            maxfd = sockets[j];
    }

    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    printf("Đang theo dõi tối đa %d socket (thời gian chờ 3 giây)...\n", i);

    int ready = select(maxfd + 1, &read_fds, NULL, NULL, &timeout);

    if (ready < 0)
    {
        perror("Lỗi khi gọi select");
    }
    else if (ready == 0)
    {
        printf("Hết thời gian chờ: không có socket nào sẵn sàng.\n");
    }
    else
    {
        printf("Có %d socket sẵn sàng để đọc.\n", ready);
    }

    for (int j = 0; j < i; j++)
        close(sockets[j]);

    printf("Đã đóng tất cả socket. Kết thúc chương trình.\n");
    return 0;
}
