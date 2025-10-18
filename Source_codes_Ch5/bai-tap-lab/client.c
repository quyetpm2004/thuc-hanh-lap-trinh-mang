#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    fd_set readfds;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Không thể tạo socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Không thể kết nối server");
        exit(EXIT_FAILURE);
    }

    printf("Đã kết nối tới server.\n");
    printf("Nhập tên người dùng của bạn: ");
    fflush(stdout);

    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Xóa ký tự xuống dòng
    send(sock, buffer, strlen(buffer), 0);

    printf("Chào %s! Bạn đã tham gia phòng chat.\n", buffer);
    printf("=======================================\n");

    // Bước 2: Vào vòng lặp chat
    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);    // stdin
        FD_SET(sock, &readfds); // socket
        int maxfd = sock;

        int activity = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (activity > 0)
        {
            if (FD_ISSET(sock, &readfds))
            {
                int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
                if (bytes <= 0)
                {
                    printf("Mất kết nối tới server.\n");
                    break;
                }
                buffer[bytes] = '\0';
                printf("%s", buffer);
            }
            if (FD_ISSET(0, &readfds))
            {
                fgets(buffer, sizeof(buffer), stdin);
                send(sock, buffer, strlen(buffer), 0);
            }
        }
    }

    close(sock);
    return 0;
}
