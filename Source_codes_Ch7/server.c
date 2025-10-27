#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int check_winner(int board[3][3]);
void send_state_update(int client1, int client2, int board[3][3]);
void notify_turn(int client_sock, int player_num);
void notify_turn_error(int client_sock, int player_num);

int main()
{
    int tcp_sock, client1_sock, client2_sock;
    struct sockaddr_in address, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE] = {0};

    int board[3][3] = {0};
    int current_player = 1;
    int move_count = 0;

    // Tạo socket
    if ((tcp_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(tcp_sock, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    listen(tcp_sock, 2);
    printf("Server đang chờ 2 người chơi...\n");

    client1_sock = accept(tcp_sock, (struct sockaddr *)&client_addr, &addr_len);
    printf("Người chơi 1 đã kết nối!\n");
    client2_sock = accept(tcp_sock, (struct sockaddr *)&client_addr, &addr_len);
    printf("Người chơi 2 đã kết nối!\n");

    int clients[2] = {client1_sock, client2_sock};

    while (1)
    {
        // Gửi thông báo lượt
        notify_turn(clients[current_player - 1], current_player);

        // Nhận MOVE
        memset(buffer, 0, BUFFER_SIZE);
        recv(clients[current_player - 1], buffer, BUFFER_SIZE, 0);
        if (buffer[0] == 0x02)
        {
            int row = buffer[1];
            int col = buffer[2];
            if (row >= 0 && row < 3 && col >= 0 && col < 3 && board[row][col] == 0)
            {
                board[row][col] = current_player;
                move_count++;
                send_state_update(client1_sock, client2_sock, board);

                int winner = check_winner(board);
                if (winner > 0 || move_count == 9)
                {
                    buffer[0] = 0x04;                      // RESULT
                    buffer[1] = (winner > 0) ? winner : 0; // 0 = hòa
                    send(client1_sock, buffer, BUFFER_SIZE, 0);
                    send(client2_sock, buffer, BUFFER_SIZE, 0);
                    break;
                }

                // Đổi lượt
                current_player = (current_player == 1) ? 2 : 1;
            }
            else
            {
                notify_turn_error(clients[current_player - 1], current_player);
                continue;
            }
        }
    }

    close(client1_sock);
    close(client2_sock);
    close(tcp_sock);
    printf("Trò chơi kết thúc.\n");
    return 0;
}

// Gửi thông báo lượt đi
void notify_turn(int client_sock, int player_num)
{
    char buffer[BUFFER_SIZE] = {0};
    buffer[0] = 0x05;
    buffer[1] = player_num;
    send(client_sock, buffer, BUFFER_SIZE, 0);
}

// Gửi thông báo lỗi
void notify_turn_error(int client_sock, int player_num)
{
    char buffer[BUFFER_SIZE] = {0};
    buffer[0] = 0x06;
    buffer[1] = player_num;
    send(client_sock, buffer, BUFFER_SIZE, 0);
}

// Gửi trạng thái bảng đến cả 2 client
void send_state_update(int client1, int client2, int board[3][3])
{
    char buffer[BUFFER_SIZE] = {0};
    buffer[0] = 0x03;
    memcpy(&buffer[1], board, sizeof(int) * 9);
    send(client1, buffer, BUFFER_SIZE, 0);
    send(client2, buffer, BUFFER_SIZE, 0);
}

// Kiểm tra thắng
int check_winner(int board[3][3])
{
    for (int i = 0; i < 3; i++)
    {
        if (board[i][0] != 0 && board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return board[i][0];
        if (board[0][i] != 0 && board[0][i] == board[1][i] && board[1][i] == board[2][i])
            return board[0][i];
    }
    if (board[0][0] != 0 && board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return board[0][0];
    if (board[0][2] != 0 && board[0][2] == board[1][1] && board[1][1] == board[2][0])
        return board[0][2];
    return 0;
}
