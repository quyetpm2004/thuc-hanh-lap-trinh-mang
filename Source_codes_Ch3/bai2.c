#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int convert_ipv4(const char *ip_str) {
    struct in_addr ipv4_addr;    // Structure to hold the IPv4 address

    // Convert the IPv4 address from string to binary form
    if (inet_pton(AF_INET, ip_str, &ipv4_addr) == 1) {
        printf("inet_pton (IPv4): Successfully converted IP address: %s\n", ip_str);
    } else {
        return 0;
    }

    char ip_str_converted[INET_ADDRSTRLEN];  // Buffer to hold the converted IP address back to string

    // Convert the binary IPv4 address back to string form
    if (inet_ntop(AF_INET, &ipv4_addr, ip_str_converted, INET_ADDRSTRLEN)) {
        printf("inet_ntop (IPv4): Converted back to string IP address: %s\n", ip_str_converted);
    } else {
        return 0;
    }
    return 1;
}

int convert_ipv6(const char *ip_str) {
    struct in6_addr ipv6_addr;   // Structure to hold the IPv6 address

    // Convert the IPv6 address from string to binary form
    if (inet_pton(AF_INET6, ip_str, &ipv6_addr) == 1) {
        printf("inet_pton (IPv6): Successfully converted IP address: %s\n", ip_str);
    } else {
        return 0;
    }

    char ip_str_converted[INET6_ADDRSTRLEN]; // Buffer to hold the converted IP address back to string

    // Convert the binary IPv6 address back to string form
    if (inet_ntop(AF_INET6, &ipv6_addr, ip_str_converted, INET6_ADDRSTRLEN)) {
        printf("inet_ntop (IPv6): Converted back to string IP address: %s\n", ip_str_converted);
    } else {
        return 0;
    }
    return 1;
}

int main() {
    printf("Enter a address: \n");
    char ip_str[INET6_ADDRSTRLEN];
    if (scanf("%45s", ip_str) != 1) {  // check error if length of ip_str > 46
        printf("IP input invalid\n");
        return 1;
    }
    if(convert_ipv4(ip_str)) {
    // Demonstrate IPv4 conversion
        printf("Done!\n");
    } else if(convert_ipv6(ip_str)) {
    // Demonstrate IPv6 conversion
        printf("Done!\n");
    } else {
        printf("IP input invalid\n");
    }

    return 0;
}
