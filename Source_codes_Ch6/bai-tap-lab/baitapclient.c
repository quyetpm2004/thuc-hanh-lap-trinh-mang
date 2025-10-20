#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAXLINE 1024

void xor_cipher(char *data, char key)
{
    for (int i = 0; data[i] != '\0'; i++)
    {
        data[i] ^= key;
    }
}

int main()
{
    char key = 'A';
    int sockfd;
    char buffer[MAXLINE];  // Thông điệp ban đầu của server
    char buffer2[MAXLINE]; // Thông điệp xác nhận từ server
    const char *message = "Hello from client";
    struct sockaddr_in servaddr, recv_servaddr;

    // Tạo socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&recv_servaddr, 0, sizeof(recv_servaddr));

    // Cấu hình địa chỉ server
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Gửi thông điệp ban đầu đến server
    sendto(sockfd, message, strlen(message), 0,
           (const struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Message sent to server.\n");

    // Nhận phản hồi đầu tiên
    int n;
    socklen_t len = sizeof(recv_servaddr);
    n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&recv_servaddr, &len);
    if (n < 0)
    {
        perror("recvfrom failed");
        close(sockfd);
        return 1;
    }
    buffer[n] = '\0';

    // Lấy IP server nhận được
    char recv_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &recv_servaddr.sin_addr, recv_ip, INET_ADDRSTRLEN);

    // Xác minh địa chỉ server bằng memcmp()
    if (memcmp(&servaddr.sin_addr, &recv_servaddr.sin_addr, sizeof(servaddr.sin_addr)) == 0 &&
        memcmp(&servaddr.sin_port, &recv_servaddr.sin_port, sizeof(servaddr.sin_port)) == 0 &&
        memcmp(&servaddr.sin_family, &recv_servaddr.sin_family, sizeof(servaddr.sin_family)) == 0)
    {
        printf("Nhận từ server: %s\n", buffer);

        xor_cipher(buffer, key);
        printf("Giải mã thành: %s\n", buffer);

        // Gửi lại thông điệp đã giải mã
        sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));

        // Nhận phản hồi xác nhận lần 2
        int m = recvfrom(sockfd, buffer2, MAXLINE, 0, (struct sockaddr *)&recv_servaddr, &len);
        if (m < 0)
        {
            perror("recvfrom failed");
        }
        else
        {
            buffer2[m] = '\0';
            printf("Server trả lời: %s\n", buffer2);
        }
    }
    else
    {
        printf("Received message from unexpected server (IP: %s, Port: %d).\n",
               recv_ip, ntohs(recv_servaddr.sin_port));
    }

    close(sockfd);
    return 0;
}
