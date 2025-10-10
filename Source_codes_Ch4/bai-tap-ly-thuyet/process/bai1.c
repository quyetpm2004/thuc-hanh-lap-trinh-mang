#include <stdio.h> 
#include <unistd.h> 
int main() { 
pid_t pid = fork(); 
if (pid == 0) { 
printf("Tiến trình con\n"); 
} else if (pid > 0) { 
printf("Tiến trình cha\n"); 
} else { 
perror("fork thất bại"); 
} 
return 0; 
}