// client.c
// Build: gcc -O2 -Wall -Wextra -o client client.c
// Run:   ./client <server_ip> <port>
// Ex:    ./client 127.0.0.1 5000
//
// Cách làm: gửi từng dòng (kết thúc bằng '\n'), chờ nhận lại đến khi thấy '\n' rồi in ra.

#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 4096

// Gửi toàn bộ len byte trong buf
static ssize_t send_all(int fd, const void *buf, size_t len) {
    const char *p = (const char *)buf;
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, p + sent, len - sent, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (n == 0) break;
        sent += (size_t)n;
    }
    return (ssize_t)sent;
}

// Nhận cho đến khi gặp '\n' hoặc đầy buffer - 1; trả về số byte đã nhận, null-terminated
static ssize_t recv_line(int fd, char *buf, size_t cap) {
    size_t used = 0;
    while (used + 1 < cap) {
        char c;
        ssize_t n = recv(fd, &c, 1, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (n == 0) {
            // server đóng
            break;
        }
        buf[used++] = c;
        if (c == '\n') break;
    }
    buf[used] = '\0';
    return (ssize_t)used;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *server_ip = argv[1];
    int port = atoi(argv[2]);
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Invalid port\n");
        return EXIT_FAILURE;
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in srv;
    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_port = htons((uint16_t)port);
    if (inet_pton(AF_INET, server_ip, &srv.sin_addr) != 1) {
        fprintf(stderr, "Invalid IP address: %s\n", server_ip);
        close(fd);
        return EXIT_FAILURE;
    }

    if (connect(fd, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
        perror("connect");
        close(fd);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "[*] Connected to %s:%d\n", server_ip, port);
    fprintf(stderr, "Type message and press Enter. Type \\quit to exit.\n");

    char line[BUF_SIZE];
    char recvbuf[BUF_SIZE];

    while (true) {
        // Prompt
        fprintf(stdout, "> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) {
            // EOF
            break;
        }

        // Nếu người dùng gõ "\quit" (có thể kèm newline)
        if (strncmp(line, "\\quit", 5) == 0) {
            break;
        }

        size_t len = strlen(line);
        // Đảm bảo có newline: nếu người dùng nhập quá dài, fgets đã cắt; vẫn gửi phần có sẵn
        if (len == 0 || line[len - 1] != '\n') {
            // nếu chưa có newline, thêm nếu còn chỗ; nếu không, cứ gửi như vậy
            if (len + 1 < sizeof(line)) {
                line[len++] = '\n';
                line[len] = '\0';
            }
        }

        if (send_all(fd, line, len) < 0) {
            perror("send");
            break;
        }

        // Nhận lại 1 dòng (đến '\n')
        ssize_t r = recv_line(fd, recvbuf, sizeof(recvbuf));
        if (r < 0) {
            perror("recv");
            break;
        }
        if (r == 0) {
            fprintf(stderr, "Server closed connection.\n");
            break;
        }

        // In thông điệp đã echo
        fprintf(stdout, "Echo: %s", recvbuf); // recvbuf đã chứa '\n' nếu server gửi lại
    }

    shutdown(fd, SHUT_RDWR);
    close(fd);
    fprintf(stderr, "[*] Disconnected.\n");
    return EXIT_SUCCESS;
}
