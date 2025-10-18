#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

int main()
{
    fd_set read_fds;
    struct timeval timeout;
    int counter = 1;

    printf("Dynamic timeout example using select().\n");
    printf("Each loop will set a different timeout value.\n");

    while (1)
    {
        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds); // Monitor stdin (keyboard input)

        // Change timeout dynamically each iteration
        timeout.tv_sec = counter; // timeout seconds increase each time
        timeout.tv_usec = 0;

        printf("\nWaiting for input... (timeout = %d sec)\n", counter);

        int ret = select(1, &read_fds, NULL, NULL, &timeout);

        if (ret < 0)
        {
            perror("select error");
            break;
        }
        else if (ret == 0)
        {
            printf("Timeout expired after %d seconds.\n", counter);
        }
        else
        {
            if (FD_ISSET(0, &read_fds))
            {
                char buffer[256];
                fgets(buffer, sizeof(buffer), stdin);
                printf("You typed: %s", buffer);
            }
        }

        // Update timeout dynamically for the next loop
        counter++;
        if (counter > 5)
            counter = 1; // reset timeout after 5 seconds
    }

    return 0;
}
