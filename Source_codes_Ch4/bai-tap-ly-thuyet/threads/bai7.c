#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

void *handle_client(void *client_socket);

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id;

    // Tạo socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Lỗi tạo socket");
        exit(1);
    }

    // Thiết lập địa chỉ server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket với địa chỉ
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Lỗi bind");
        close(server_fd);
        exit(1);
    }

    // Lắng nghe kết nối
    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("Lỗi listen");
        close(server_fd);
        exit(1);
    }

    printf("Server đang lắng nghe tại cổng %d...\n", PORT);

    // Chấp nhận và tạo luồng xử lý từng client
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("Lỗi accept");
            continue;
        }

        printf("Kết nối mới từ %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        int *new_sock = malloc(sizeof(int));
        *new_sock = client_fd;

        if (pthread_create(&thread_id, NULL, handle_client, (void *)new_sock) != 0) {
            perror("Không thể tạo luồng");
            free(new_sock);
            close(client_fd);
        }

        pthread_detach(thread_id);
    }

    close(server_fd);
    return 0;
}

// Hàm xử lý client trong luồng riêng
void *handle_client(void *client_socket) {
    int sock = *(int *)client_socket;
    free(client_socket);
    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';
        printf("Nhận từ client: %s\n", buffer);

        // Gửi lại cho client (echo)
        send(sock, buffer, strlen(buffer), 0);
    }

    printf("Client đã ngắt kết nối.\n");
    close(sock);
    pthread_exit(NULL);
}
