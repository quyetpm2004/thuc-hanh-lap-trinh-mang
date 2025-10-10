#include <pthread.h> 
#include <stdio.h> 
#define NUM_THREAD 3

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 

int counter = 1; 
 

void *write_file(void *arg) { 
pthread_mutex_lock(&lock); 
fprintf(arg, "Luong so %d ghi vao file\n", counter);
counter++;
pthread_mutex_unlock(&lock); 
return NULL; 
}


int main() { 
    FILE *filePointer;

    filePointer = fopen("output.txt", "w");

    pthread_t tids[NUM_THREAD]; 
for (int i = 0; i < NUM_THREAD; i++) { 
pthread_create(&tids[i], NULL, write_file, filePointer); 
} 
for (int i = 0; i < NUM_THREAD; i++) pthread_join(tids[i], NULL); 

    fclose(filePointer);

return 0; 
}