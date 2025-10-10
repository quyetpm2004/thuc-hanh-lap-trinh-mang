#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    for (int i = 0; i < 5; i++) {
        pid_t pid = fork();

        if (pid == 0) {  // Tiến trình con
            printf("Tiến trình con %d (PID=%d, PPID=%d)\n", i, getpid(), getppid());
            _exit(0); // Kết thúc con ngay lập tức
        }
        else if (pid < 0) {
            perror("fork thất bại");
            return 1;
        }
    }

    // Tiến trình cha chờ các con
    for (int i = 0; i < 5; i++) {
        wait(NULL);
    }

    printf("Tiến trình cha kết thúc\n");
    return 0;
}
