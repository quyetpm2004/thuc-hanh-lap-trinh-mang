
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 8080
#define NPROCESS 3
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
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Lỗi bind");
        exit(1);
    }

    // Listen
    listen(server_fd, 5);
    printf("Server (fork trước accept) đang lắng nghe tại cổng %d...\n", PORT);

    // Tạo N tiến trình con — mỗi tiến trình đều gọi accept()
    for (int i = 0; i < NPROCESS; i++) {
        pid_t pid = fork();
        if (pid == 0) { // tiến trình con
            while (1) {
                client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
                if (client_fd < 0) continue;

                printf("Process %d xử lý client %s:%d\n", getpid(),
                       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
                buffer[bytes] = '\0';
                printf("[%d] Nhận: %s\n", getpid(), buffer);

                send(client_fd, "Hello from pre-fork server!\n", 29, 0);
                close(client_fd);
            }
            exit(0);
        }
    }

    // Tiến trình cha chờ con
    for (int i = 0; i < NPROCESS; i++)
        wait(NULL);

    close(server_fd);
    return 0;
}
