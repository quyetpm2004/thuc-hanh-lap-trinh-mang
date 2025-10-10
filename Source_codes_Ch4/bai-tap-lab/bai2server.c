// quiz_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <ctype.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define NUM_QUESTIONS 10

// Cấu trúc câu hỏi trắc nghiệm
typedef struct {
    char question[256];
    char options[4][128];
    int correct; // chỉ số đáp án đúng (0-3)
} QuizQuestion;

// Danh sách 10 câu hỏi
QuizQuestion quiz[NUM_QUESTIONS] = {
    {"Thủ đô của Việt Nam là gì?", {"Hà Nội", "TP.HCM", "Huế", "Đà Nẵng"}, 0},
    {"2 + 2 = ?", {"3", "4", "5", "6"}, 1},
    {"Ngôn ngữ lập trình nào chạy trên JVM?", {"Python", "C", "Java", "Go"}, 2},
    {"Tác giả 'Truyện Kiều' là ai?", {"Nguyễn Du", "Nguyễn Trãi", "Hồ Xuân Hương", "Xuân Diệu"}, 0},
    {"Biển lớn nhất thế giới là?", {"Thái Bình Dương", "Đại Tây Dương", "Ấn Độ Dương", "Bắc Băng Dương"}, 0},
    {"Số nguyên tố nhỏ nhất là?", {"0", "1", "2", "3"}, 2},
    {"Hệ điều hành Linux thuộc loại nào?", {"Mã nguồn mở", "Thương mại", "Miễn phí", "Đóng mã"}, 0},
    {"Công ty tạo ra Windows là?", {"Google", "Microsoft", "Apple", "IBM"}, 1},
    {"Cầu thủ nổi tiếng Messi đến từ nước nào?", {"Bồ Đào Nha", "Argentina", "Pháp", "Tây Ban Nha"}, 1},
    {"Tốc độ ánh sáng xấp xỉ?", {"300000 km/s", "150000 km/s", "100000 km/s", "500000 km/s"}, 0}
};

// --- Hàm xử lý zombie ---
void sigchld_handler(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// --- Xáo trộn thứ tự các đáp án ---
void shuffle_options(char options[4][128], int *correct_index) {
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        char temp[128];
        strcpy(temp, options[i]);
        strcpy(options[i], options[j]);
        strcpy(options[j], temp);
        if (*correct_index == i)
            *correct_index = j;
        else if (*correct_index == j)
            *correct_index = i;
    }
}

// --- Hàm xử lý mỗi client ---
void handle_client(int connfd) {
    srand(time(NULL) ^ getpid()); // seed ngẫu nhiên theo PID
    int score = 0;
    char buffer[BUFFER_SIZE];

    for (int i = 0; i < NUM_QUESTIONS; i++) {
        QuizQuestion q = quiz[i];
        shuffle_options(q.options, &q.correct);

        // Gửi câu hỏi và 4 đáp án
        snprintf(buffer, sizeof(buffer),
                 "Câu %d: %s\nA. %s\nB. %s\nC. %s\nD. %s\nNhập đáp án (A/B/C/D): ",
                 i + 1, q.question,
                 q.options[0], q.options[1], q.options[2], q.options[3]);
        send(connfd, buffer, strlen(buffer), 0);

        // Nhận câu trả lời
        int n = recv(connfd, buffer, BUFFER_SIZE, 0);
        if (n <= 0) break;
        buffer[n] = '\0';

        // Chuẩn hóa nhập liệu
        char ans = toupper(buffer[0]);
        int choice = (ans == 'A') ? 0 : (ans == 'B') ? 1 : (ans == 'C') ? 2 : (ans == 'D') ? 3 : -1;

        if (choice == q.correct)
            score++;

        // Thông báo đúng/sai
        snprintf(buffer, sizeof(buffer), "=> %s!\n\n", (choice == q.correct) ? "Đúng" : "Sai");
        send(connfd, buffer, strlen(buffer), 0);
    }

    // Gửi điểm tổng kết
    snprintf(buffer, sizeof(buffer), "Hoàn thành! Tổng điểm của bạn là: %d / %d\n", score, NUM_QUESTIONS);
    send(connfd, buffer, strlen(buffer), 0);

    close(connfd);
    printf("Client đã hoàn thành bài và ngắt kết nối.\n");
    exit(0);
}

// --- Hàm main ---
int main() {
    int listenfd, connfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    pid_t pid;

    signal(SIGCHLD, sigchld_handler);

    // Tạo socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    if (listen(listenfd, 5) < 0) {
        perror("Listen failed");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);


    while (1) {
        connfd = accept(listenfd, (struct sockaddr *)&client_addr, &addr_len);
        if (connfd < 0) {
            perror("Accept failed");
            continue;
        }

        pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            close(connfd);
        } else if (pid == 0) {
            close(listenfd);
            handle_client(connfd);
        } else {
            close(connfd);
        }
    }

    return 0;
}
