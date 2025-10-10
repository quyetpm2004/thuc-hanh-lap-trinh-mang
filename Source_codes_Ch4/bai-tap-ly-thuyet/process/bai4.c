#include <stdio.h> 
#include <unistd.h> 
int main() { 
if (fork() == 0) { 
printf("Tiến trình con kết thúc\n"); 
_exit(0); 
} else { 
sleep(10); // Tiến trình cha tạm dừng, con trở thành zombie 
} 
return 0; 
}