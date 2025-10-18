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

    int result = select(sockfd + 1, &read_fds, NULL, NULL, NULL); // select() sẽ chờ vô thời hạn cho đến khi có FD sẵn sàng

    if (result > 0) {
        if (FD_ISSET(sockfd, &read_fds)) {
            printf("Socket san sang de doc!\n");
        }
    } else if (result == 0) {
        printf("Khong co Socket nao san sang\n");
    } else {
        printf("Co loi xay ra\n");
    }

    return 0;
}