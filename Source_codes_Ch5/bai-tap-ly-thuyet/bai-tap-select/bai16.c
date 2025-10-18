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

    if (sockfd1 < 0 || sockfd2 < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Tạo tập fd_set
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(sockfd1, &read_fds);
    FD_SET(sockfd2, &read_fds);

    // Tìm socket lớn nhất
    int maxfd = (sockfd1 > sockfd2) ? sockfd1 : sockfd2;

    // Thiết lập thời gian chờ 5 giây
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    printf("Đang chờ socket sẵn sàng (tối đa 5 giây)...\n");

    // Gọi select()
    int result = select(maxfd + 1, &read_fds, NULL, NULL, &timeout);

    if (result < 0)
    {
        perror("select");
        exit(EXIT_FAILURE);
    }
    else if (result == 0)
    {
        for (int i = 0; i <= maxfd; i++)
        {
            if (!FD_ISSET(i, &read_fds))
            {
                printf("Đã đóng socket với fd=%d vì không sẵn sàng.\n", i);
                close(i);
            }
        }
    }
    else
    {
        printf("%d socket sẵn sàng.\n", result);

        if (FD_ISSET(sockfd1, &read_fds))
            printf("→ Socket 1 (fd=%d) sẵn sàng đọc.\n", sockfd1);

        if (FD_ISSET(sockfd2, &read_fds))
            printf("→ Socket 2 (fd=%d) sẵn sàng đọc.\n", sockfd2);
    }

    // Đóng socket
    close(sockfd1);
    close(sockfd2);

    return 0;
}
