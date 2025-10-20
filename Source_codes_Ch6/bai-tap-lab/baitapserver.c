#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#include <sys/select.h>
#include <sys/time.h>

#define PORT 8080
#define MAXLINE 1024

void xor_cipher(char *data, char key)
{
    for (int i = 0; data[i] != '\0'; i++)
    {
        data[i] ^= key;
    }
}

char *randomString(const char *arr[], int size)
{
    int index = rand() % size; // random chỉ số trong mảng
    return (char *)arr[index];
}

int main()
{
    fd_set readfds;
    struct timeval tv;
    const char *words[] = {
        "banana",
        "apple",
        "cherry",
        "orange",
        "grape"};
    int size = sizeof(words) / sizeof(words[0]);
    char key = 'A';
    int sockfd;
    char buffer[MAXLINE];
    char giaimatuclient[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    int n;                           // biến nhận khi client kết nối đến
    socklen_t len = sizeof(cliaddr); // len is value/result

    int m; // biến nhận khi client giải mã và gửi về

    printf("Server is running...\n");

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        int ready = select(sockfd + 1, &readfds, NULL, NULL, &tv);
        if (ready < 0)
        {
            perror("select");
            break;
        }
        else if (ready == 0)
        {
            printf("Chưa có client nào kết nào đến! (timeout = 5s)\n");
        }
        else if (ready > 0 && FD_ISSET(sockfd, &readfds))
        {
            memset(&cliaddr, 0, sizeof(cliaddr));
            // Nhận thông điệp từ client
            n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
            buffer[n] = '\0';

            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(cliaddr.sin_addr), client_ip, INET_ADDRSTRLEN);

            printf("\nClient IP: %s, Port: %d\n", client_ip, ntohs(cliaddr.sin_port));
            printf("Client message: %s\n", buffer);

            // Random lại chuỗi mỗi lần có client gửi tin
            const char *data_const = randomString(words, size);
            char data[MAXLINE];
            strcpy(data, data_const);

            // Mã hóa dữ liệu
            xor_cipher(data, key);

            // Gửi dữ liệu mã hóa cho client
            sendto(sockfd, data, strlen(data), 0, (const struct sockaddr *)&cliaddr, len);
            printf("Đã gửi thông điệp %s mã hóa là %s đến client\n", data_const, data);

            // Nhận dữ liệu client giải mã gửi lại
            m = recvfrom(sockfd, (char *)giaimatuclient, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
            giaimatuclient[m] = '\0';
            printf("Client giải mã: %s\n", giaimatuclient);

            // Giải mã lại trên server để kiểm tra
            xor_cipher(data, key);

            if (strcmp(data, giaimatuclient) == 0)
            {
                char *result = "Chúc mừng bạn đã giải mã đúng";
                sendto(sockfd, result, strlen(result), 0, (const struct sockaddr *)&cliaddr, len);
            }
            else
            {
                char *result = "Rất tiếc bạn đã giải mã sai";
                sendto(sockfd, result, strlen(result), 0, (const struct sockaddr *)&cliaddr, len);
            }
        }
    }

    close(sockfd);
    return 0;
}
