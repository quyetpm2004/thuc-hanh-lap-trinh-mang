#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>

int main () {
    fd_set read_fds; 
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    FD_ZERO(&read_fds);    // Khởi tạo tập FD rỗng 
    FD_SET(sockfd, &read_fds);  // Thêm sockfd vào tập
}