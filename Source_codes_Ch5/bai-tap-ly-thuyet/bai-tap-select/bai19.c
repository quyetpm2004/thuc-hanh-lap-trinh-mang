#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>

int main()
{
    int sockets[FD_SETSIZE];
    fd_set read_fds;
    struct timeval start, end;
    int i;

    for (i = 0; i < FD_SETSIZE; i++)
    {
        sockets[i] = socket(AF_INET, SOCK_STREAM, 0);
        if (sockets[i] < 0)
        {
            perror("Lỗi tạo socket");
            printf("Dừng lại ở socket #%d\n", i);
            break;
        }
    }

    FD_ZERO(&read_fds);
    int maxfd = 0;
    for (int j = 0; j < i; j++)
    {
        FD_SET(sockets[j], &read_fds);
        if (sockets[j] > maxfd)
            maxfd = sockets[j];
    }

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    gettimeofday(&start, NULL);
    int result = select(maxfd + 1, &read_fds, NULL, NULL, &timeout);
    gettimeofday(&end, NULL);

    double elapsed = (end.tv_sec - start.tv_sec) * 1000.0;
    elapsed += (end.tv_usec - start.tv_usec) / 1000.0;

    printf("Theo dõi %d socket, select() mất %.3f ms\n", i, elapsed);

    for (int j = 0; j < i; j++)
        close(sockets[j]);

    return 0;
}
