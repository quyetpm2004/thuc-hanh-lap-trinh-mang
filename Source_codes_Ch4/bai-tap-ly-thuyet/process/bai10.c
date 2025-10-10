#include <stdio.h> 
#include <unistd.h> 
int main() { 
pid_t pid1 = fork(); 
if (pid1 == 0) { 
    printf("Tiến trình con 1, PID = %d, Tiến trình cha, PPID = %d\n", getpid(), getppid()); 
} else { 
    pid_t pid2 = fork(); 
    if (pid2 == 0) { 
        printf("Tiến trình con 2, PID = %d, Tiến trình cha, PPID = %d\n", getpid(), getppid()); 
    } else { 
        printf("Tiến trình cha, PID = %d\n", getpid()); 
        wait(NULL); wait(NULL); 
    } 
}
return 0; 
}