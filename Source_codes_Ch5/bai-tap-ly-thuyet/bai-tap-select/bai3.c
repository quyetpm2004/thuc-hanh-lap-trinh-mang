#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>

int main () {
    fd_set read_fds; // Một tập các file descriptor
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    FD_ZERO(&read_fds);    // Khởi tạo tập FD rỗng 
    FD_SET(sockfd, &read_fds);  // Thêm sockfd vào tập


    if (FD_ISSET(sockfd, &read_fds)) { 
        printf("Socket %d có sẵn để đọc\n", sockfd); 
    }
    FD_CLR(sockfd, &read_fds); 
    printf("Sau khi xóa socket\n");
    if (FD_ISSET(sockfd, &read_fds)) { 
        printf("Socket %d có sẵn để đọc\n", sockfd); 
    } else {
        printf("Socket nãy đã bị xóa\n"); 
    }
    return 0;
}