#include <stdio.h>

int main() {
    FILE *filePointer;
    int age = 30;
    char city[] = "New York";

    // Open a file in write mode ("w")
    filePointer = fopen("output.txt", "w");

    // Check if the file was opened successfully
    if (filePointer == NULL) {
        printf("Error opening file!\n");
        return 1; // Indicate an error
    }

    // Write formatted data to the file
    fprintf(filePointer, "User Age: %d\n", age);
    fprintf(filePointer, "User City: %s\n", city);

    // Close the file
    fclose(filePointer);

    printf("Data written to output.txt successfully.\n");

    return 0; // Indicate success
}