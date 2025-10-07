// server.c
// Build: gcc -O2 -Wall -Wextra -pthread -o server server.c
// Run:   ./server <port>
// Ex:    ./server 5000

#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BACKLOG 128
#define BUF_SIZE 4096

static volatile sig_atomic_t g_stop = 0;
static int listen_fd = -1;

static void handle_sigint(int sig) {
    (void)sig;
    g_stop = 1;
    if (listen_fd != -1) {
        close(listen_fd); // Thoát accept() đang chờ
    }
}

struct client_ctx {
    int fd;
    struct sockaddr_in addr;
};

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

static void *client_thread(void *arg) {
    struct client_ctx *ctx = (struct client_ctx *)arg;
    int fd = ctx->fd;

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ctx->addr.sin_addr, ip, sizeof(ip));
    uint16_t port = ntohs(ctx->addr.sin_port);
    fprintf(stderr, "[+] Client connected: %s:%u\n", ip, port);
    free(ctx);

    char buf[BUF_SIZE];
    for (;;) {
        ssize_t n = recv(fd, buf, sizeof(buf), 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("recv");
            break;
        }
        if (n == 0) {
            // client đóng kết nối
            break;
        }
        // Echo đúng số byte nhận được
        if (send_all(fd, buf, (size_t)n) < 0) {
            perror("send");
            break;
        }
    }

    fprintf(stderr, "[-] Client disconnected: %s:%u\n", ip, port);
    close(fd);
    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int port = atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Invalid port\n");
        return EXIT_FAILURE;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    int yes = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        perror("setsockopt SO_REUSEADDR");
        // không return — tiếp tục chạy được
    }

    struct sockaddr_in srv;
    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = htonl(INADDR_ANY);
    srv.sin_port = htons((uint16_t)port);

    if (bind(listen_fd, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
        perror("bind");
        close(listen_fd);
        return EXIT_FAILURE;
    }

    if (listen(listen_fd, BACKLOG) < 0) {
        perror("listen");
        close(listen_fd);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "[*] Echo server listening on port %d ... (Ctrl+C to stop)\n", port);

    while (!g_stop) {
        struct sockaddr_in cli;
        socklen_t cli_len = sizeof(cli);
        int fd = accept(listen_fd, (struct sockaddr *)&cli, &cli_len);
        if (fd < 0) {
            if (errno == EINTR && g_stop) break;
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }

        struct client_ctx *ctx = (struct client_ctx *)malloc(sizeof(*ctx));
        if (!ctx) {
            fprintf(stderr, "malloc failed\n");
            close(fd);
            continue;
        }
        ctx->fd = fd;
        ctx->addr = cli;

        pthread_t th;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // khi client huy thi luong nay tu huy
        if (pthread_create(&th, &attr, client_thread, ctx) != 0) {
            perror("pthread_create");
            close(fd);
            free(ctx);
        }
        pthread_attr_destroy(&attr);
    }

    if (listen_fd != -1) close(listen_fd);
    fprintf(stderr, "[*] Server stopped.\n");
    return EXIT_SUCCESS;
}
