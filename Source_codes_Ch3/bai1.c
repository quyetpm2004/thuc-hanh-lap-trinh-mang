#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>  // For inet_pton and sockaddr_in

int main() {
    struct sockaddr_in server_addr1;
    struct sockaddr_in server_addr2;

    // Zero out the sockaddr_in structure
    memset(&server_addr1, 0, sizeof(server_addr1));
    memset(&server_addr2, 0, sizeof(server_addr2));

    // Set the address family to IPv4
    server_addr1.sin_family = AF_INET;
    server_addr2.sin_family = AF_INET;

    // Set the port number (use htons to convert to network byte order)
    server_addr1.sin_port = htons(8080);
    server_addr2.sin_port = htons(8000);

    // Set the IP address (use inet_pton to convert from text to binary form)
    if (inet_pton(AF_INET, "192.168.1.1", &server_addr1.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }

    if (inet_pton(AF_INET, "192.168.1.1", &server_addr2.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }

    // Print the initialized structure
    printf("server_addr1 sockaddr_in structure initialized:\n");
    printf("  Family      : AF_INET (IPv4)\n");
    printf("  Port        : %d\n", ntohs(server_addr1.sin_port));  // convert back to host byte order for printing
    printf("  IP Address  : 192.168.1.1\n");

    // Print the initialized structure
    printf("server_addr2 sockaddr_in structure initialized:\n");
    printf("  Family      : AF_INET (IPv4)\n");
    printf("  Port        : %d\n", ntohs(server_addr2.sin_port));  // convert back to host byte order for printing
    printf("  IP Address  : 192.168.1.1\n");

    return 0;
}
