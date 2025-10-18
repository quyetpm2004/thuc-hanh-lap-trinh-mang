#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    fd_set read_fds;
    int maxfd;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080);

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect failed");
        close(sockfd);
        exit(1);
    }

    printf("Connected to server. Type messages below.\n");
    printf("Anything typed in stdin will be sent to the server.\n");

    while (1)
    {
        // Clear and set monitored descriptors
        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);      // stdin (keyboard)
        FD_SET(sockfd, &read_fds); // socket connection

        // Determine the maximum descriptor value
        maxfd = (sockfd > 0) ? sockfd : 0;

        // Wait for input from either stdin or socket
        int activity = select(maxfd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0)
        {
            perror("select error");
            break;
        }

        // Check if user typed something
        if (FD_ISSET(0, &read_fds))
        {
            char input[1024];
            if (fgets(input, sizeof(input), stdin) != NULL)
            {
                send(sockfd, input, strlen(input), 0);
            }
        }

        // Check if there is data from server
        if (FD_ISSET(sockfd, &read_fds))
        {
            char buffer[1024];
            int bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
            if (bytes <= 0)
            {
                printf("Server closed the connection.\n");
                break;
            }
            else
            {
                buffer[bytes] = '\0';
                printf("Received from server: %s", buffer);
            }
        }
    }

    close(sockfd);
    return 0;
}
