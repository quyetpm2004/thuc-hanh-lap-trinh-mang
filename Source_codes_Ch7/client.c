#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void print_board(int board[3][3]);

int main()
{
    int tcp_sock = 0;
    int row, col;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE] = {0};
    int board[3][3];

    if ((tcp_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket creation error\n");
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0)
    {
        printf("Invalid address\n");
        return -1;
    }

    if (connect(tcp_sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("Connection Failed\n");
        return -1;
    }

    printf("Đã kết nối đến server. Chờ lượt chơi...\n");

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        recv(tcp_sock, buffer, BUFFER_SIZE, 0);

        if (buffer[0] == 0x05)
        {
            printf("Đến lượt bạn (người chơi %d)!\n", buffer[1]);
            printf("Nhập hàng và cột (0-2): ");
            scanf("%d %d", &row, &col);
            buffer[0] = 0x02; // MOVE
            buffer[1] = row;
            buffer[2] = col;
            send(tcp_sock, buffer, BUFFER_SIZE, 0);
        }
        else if (buffer[0] == 0x03)
        {
            memcpy(board, &buffer[1], sizeof(int) * 9);
            print_board(board);
        }
        else if (buffer[0] == 0x04)
        {
            int result = buffer[1];
            if (result == 0)
                printf("Trò chơi hòa!\n");
            else
                printf("Người chơi %d thắng!\n", result);
            break;
        }
    }

    close(tcp_sock);
    return 0;
}

void print_board(int board[3][3])
{
    printf("\nBảng cờ:\n");
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            char symbol = (board[i][j] == 1) ? 'X' : (board[i][j] == 2) ? 'O'
                                                                        : '.';
            printf("%c ", symbol);
        }
        printf("\n");
    }
    printf("\n");
}
