// quiz_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Đã kết nối tới server.\n\n");

    while (1) {
        int n = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (n <= 0) break;
        buffer[n] = '\0';
        printf("%s", buffer);

        // Nếu đây là phần câu hỏi thì cho người dùng nhập
        if (strstr(buffer, "Nhập đáp án") != NULL) {
            printf("> ");
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            send(sock, buffer, strlen(buffer), 0);
        }
    }

    printf("\nKết thúc trò chơi.\n");
    close(sock);
    return 0;
}
