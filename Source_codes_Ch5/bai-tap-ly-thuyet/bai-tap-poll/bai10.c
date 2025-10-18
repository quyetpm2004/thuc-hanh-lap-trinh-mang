#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

#define PORT 8080
#define MAX_CLIENTS 100

int main()
{
    int listen_fd, new_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    struct pollfd fds[MAX_CLIENTS];
    int num_fds = 1;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        perror("Không tạo được socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Lỗi bind");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(listen_fd, 5) < 0)
    {
        perror("Lỗi listen");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    // Thêm socket lắng nghe vào danh sách
    fds[0].fd = listen_fd;
    fds[0].events = POLLIN;

    printf("Server đang chạy, chờ kết nối ở cổng %d...\n", PORT);

    while (1)
    {
        int ret = poll(fds, num_fds, -1);
        if (ret < 0)
        {
            perror("Lỗi poll");
            break;
        }

        // Kiểm tra socket lắng nghe
        if (fds[0].revents & POLLIN)
        {
            new_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
            if (new_fd >= 0)
            {
                printf("Client mới kết nối: FD=%d\n", new_fd);
                fds[num_fds].fd = new_fd;
                fds[num_fds].events = POLLIN;
                num_fds++;
            }
        }

        // Kiểm tra dữ liệu từ các client
        for (int i = 1; i < num_fds; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                char buffer[1024];
                int bytes = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
                if (bytes <= 0)
                {
                    printf("Client FD=%d đã đóng kết nối\n", fds[i].fd);
                    close(fds[i].fd);
                    fds[i] = fds[num_fds - 1];
                    num_fds--;
                    i--; // kiểm tra lại vị trí này
                }
                else
                {
                    buffer[bytes] = '\0';
                    printf("Nhận từ client FD=%d: %s\n", fds[i].fd, buffer);
                    send(fds[i].fd, buffer, bytes, 0); // Gửi lại dữ liệu (echo)
                }
            }
        }
    }

    close(listen_fd);
    return 0;
}
