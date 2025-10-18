#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>

#define PORT 8080
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define USERNAME_LEN 50

typedef struct
{
    int fd;
    char username[USERNAME_LEN];
} Client;

void broadcast_message(int sender_fd, Client *clients, int num_clients, const char *message)
{
    for (int i = 0; i < num_clients; i++)
    {
        if (clients[i].fd > 0 && clients[i].fd != sender_fd)
        {
            send(clients[i].fd, message, strlen(message), 0);
        }
    }
}

int main()
{
    int listener, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen;
    fd_set readfds;
    int max_sd, sd, activity;
    char buffer[BUFFER_SIZE];

    Client clients[MAX_CLIENTS];

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == 0)
    {
        perror("Không thể tạo socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(listener, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Lỗi bind");
        exit(EXIT_FAILURE);
    }

    if (listen(listener, 5) < 0)
    {
        perror("Lỗi listen");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i].fd = 0;
        memset(clients[i].username, 0, USERNAME_LEN);
    }

    printf("Server đang lắng nghe trên cổng %d...\n", PORT);

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(listener, &readfds);
        max_sd = listener;

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            sd = clients[i].fd;
            if (sd > 0)
                FD_SET(sd, &readfds);
            if (sd > max_sd)
                max_sd = sd;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR))
            perror("Lỗi select");

        if (FD_ISSET(listener, &readfds))
        {
            addrlen = sizeof(address);
            new_socket = accept(listener, (struct sockaddr *)&address, &addrlen);
            if (new_socket < 0)
            {
                perror("Lỗi accept");
                exit(EXIT_FAILURE);
            }

            printf("Client mới kết nối: FD=%d, IP=%s, PORT=%d\n",
                   new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // Gửi yêu cầu nhập tên người dùng
            // char ask_name[] = "Nhập tên người dùng của bạn: ";
            // send(new_socket, ask_name, strlen(ask_name), 0);

            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (clients[i].fd == 0)
                {
                    clients[i].fd = new_socket;
                    strcpy(clients[i].username, ""); // chưa có tên
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            sd = clients[i].fd;
            if (sd > 0 && FD_ISSET(sd, &readfds))
            {
                int valread = read(sd, buffer, BUFFER_SIZE - 1);
                if (valread == 0)
                {
                    if (strlen(clients[i].username) > 0)
                    {
                        char leave_msg[200];
                        snprintf(leave_msg, sizeof(leave_msg), "<-- %s đã rời phòng chat\n", clients[i].username);
                        broadcast_message(sd, clients, MAX_CLIENTS, leave_msg);
                        printf("%s", leave_msg);
                    }
                    close(sd);
                    clients[i].fd = 0;
                    memset(clients[i].username, 0, USERNAME_LEN);
                }
                else
                {
                    buffer[valread] = '\0';
                    if (strlen(clients[i].username) == 0)
                    {
                        // Lần đầu tiên: nhận tên người dùng
                        buffer[strcspn(buffer, "\n")] = '\0';
                        strcpy(clients[i].username, buffer);

                        char join_msg[200];
                        snprintf(join_msg, sizeof(join_msg), "--> %s đã tham gia phòng chat\n", clients[i].username);
                        broadcast_message(sd, clients, MAX_CLIENTS, join_msg);
                        printf("%s", join_msg);

                        char welcome[100];
                        snprintf(welcome, sizeof(welcome), "Chào mừng %s đến với phòng chat!\n", clients[i].username);
                        send(sd, welcome, strlen(welcome), 0);
                    }
                    else
                    {
                        // Tin nhắn bình thường
                        char msg_with_name[BUFFER_SIZE + USERNAME_LEN + 5];
                        snprintf(msg_with_name, sizeof(msg_with_name), "%s: %s", clients[i].username, buffer);
                        broadcast_message(sd, clients, MAX_CLIENTS, msg_with_name);
                        printf("%s", msg_with_name);
                    }
                }
            }
        }
    }

    return 0;
}
