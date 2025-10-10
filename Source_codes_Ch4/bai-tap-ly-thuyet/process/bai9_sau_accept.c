#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Tạo socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Lỗi tạo socket");
        exit(1);
    }

    // Bind
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Listen
    listen(server_fd, 5);
    printf("Server (fork sau accept) đang lắng nghe tại cổng %d...\n", PORT);

    // Vòng lặp chính — accept trước, fork sau
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) continue;

        printf("Kết nối mới từ %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        pid_t pid = fork();
        if (pid == 0) { // Tiến trình con
            close(server_fd); // con không cần socket listen
            recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            printf("Process %d xử lý client\n", getpid());
            send(client_fd, "Hello from post-fork server!\n", 30, 0);
            close(client_fd);
            exit(0);
        } else {
            close(client_fd); // cha không xử lý client
        }
    }

    close(server_fd);
    return 0;
}
