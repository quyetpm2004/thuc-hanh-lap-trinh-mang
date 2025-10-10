#include <stdio.h> 
#include <unistd.h> 
int main() { 
if (fork() == 0) { 
printf("Tiến trình con\n"); 
} else if (fork() > 0) { 
printf("Tiến trình cha\n"); 
}
return 0; 
}