#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

int main()
{
    int listener, client;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    fd_set read_fds;
    int maxfd;

    // Tạo socket lắng nghe
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0)
    {
        perror("socket failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    bind(listener, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(listener, 5);

    printf("Server đang lắng nghe trên cổng 8080...\n");

    client = -1;

    while (1)
    {
        // Thiết lập tập các socket cần theo dõi
        FD_ZERO(&read_fds);
        FD_SET(listener, &read_fds);
        if (client != -1)
            FD_SET(client, &read_fds);

        maxfd = (listener > client) ? listener : client;

        // Chờ sự kiện trên socket
        int activity = select(maxfd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0)
        {
            perror("select error");
            break;
        }

        // Có kết nối mới đến
        if (FD_ISSET(listener, &read_fds))
        {
            client_len = sizeof(client_addr);
            client = accept(listener, (struct sockaddr *)&client_addr, &client_len);
            printf("Client mới kết nối từ %s:%d\n",
                   inet_ntoa(client_addr.sin_addr),
                   ntohs(client_addr.sin_port));
        }

        // Có dữ liệu từ client
        if (client != -1 && FD_ISSET(client, &read_fds))
        {
            char buffer[1024];
            int bytes = recv(client, buffer, sizeof(buffer) - 1, 0);
            if (bytes <= 0)
            {
                printf("Client đã ngắt kết nối.\n");
                close(client);
                client = -1;
            }
            else
            {
                buffer[bytes] = '\0';
                printf("Nhận từ client: %s\n", buffer);
                send(client, buffer, bytes, 0); // gửi lại dữ liệu (echo)
            }
        }
    }

    close(listener);
    return 0;
}
