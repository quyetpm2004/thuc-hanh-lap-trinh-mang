#include <stdio.h> 
#include <unistd.h> 
#include <sys/wait.h> 
int main() { 
if (fork() == 0) { 
printf("Tiến trình con\n"); 
_exit(0); 
} else { 
wait(NULL); // Thu dọn tiến trình con 
printf("Tiến trình cha kết thúc\n"); 
} 
return 0; 
}