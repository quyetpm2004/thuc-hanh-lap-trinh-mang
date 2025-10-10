#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int global_var = 0; // Biến toàn cục dùng chung

void* thread_func(void* arg) {
    int local_var = 0; // Biến cục bộ (riêng từng luồng)
    local_var = (int)(size_t)arg; // mỗi luồng có giá trị riêng

    printf("Thread %d: địa chỉ biến cục bộ = %p\n", local_var, &local_var);

    // Tăng biến toàn cục
    for (int i = 0; i < 5; i++) {
        global_var++;
        printf("Thread %d: global_var = %d\n", local_var, global_var);
        sleep(1);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[2];

    printf("Địa chỉ biến toàn cục = %p\n", &global_var);

    // Tạo 2 luồng
    for (int i = 0; i < 2; i++) {
        pthread_create(&threads[i], NULL, thread_func, (void*)(size_t)(i + 1));
    }

    // Chờ các luồng kết thúc
    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Kết thúc chương trình, global_var = %d\n", global_var);
    return 0;
}
