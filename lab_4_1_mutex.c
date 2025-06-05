#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define FILENAME "shared_file.txt"
#define MAX_LEN 100

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

// Багатопотокова версія
void* writer(void* arg) {
    for (int i = 0; i < 1000; i++) {
        pthread_mutex_lock(&file_mutex);
        FILE* file = fopen(FILENAME, "a");
        if (file) {
            fprintf(file, "Line %d\n", i);
            fclose(file);
        }
        pthread_mutex_unlock(&file_mutex);
    }
    return NULL;
}

void* reader(void* arg) {
    for (int i = 0; i < 1000; i++) {
        pthread_mutex_lock(&file_mutex);
        FILE* file = fopen(FILENAME, "r");
        if (file) {
            char buffer[MAX_LEN];
            while (fgets(buffer, MAX_LEN, file)) {}
            fclose(file);
        }
        pthread_mutex_unlock(&file_mutex);
    }
    return NULL;
}

// Однопотокова версія
void single_thread_file_ops() {
    for (int i = 0; i < 1000; i++) {
        FILE* file = fopen(FILENAME, "a");
        if (file) {
            fprintf(file, "Line %d\n", i);
            fclose(file);
        }
        
        file = fopen(FILENAME, "r");
        if (file) {
            char buffer[MAX_LEN];
            while (fgets(buffer, MAX_LEN, file)) {}
            fclose(file);
        }
    }
}

int main() {
    clock_t start, end;
    double cpu_time_used;
    
    // Очистимо файл перед тестами
    FILE* file = fopen(FILENAME, "w");
    if (file) fclose(file);

    // Тестуємо багатопотокову версію
    start = clock();
    
    pthread_t writer1, writer2, reader1;
    pthread_create(&writer1, NULL, writer, NULL);
    pthread_create(&writer2, NULL, writer, NULL);
    pthread_create(&reader1, NULL, reader, NULL);
    
    pthread_join(writer1, NULL);
    pthread_join(writer2, NULL);
    pthread_join(reader1, NULL);
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Multithreaded time: %.4f seconds\n", cpu_time_used);

    // Очистимо файл перед наступним тестом
    file = fopen(FILENAME, "w");
    if (file) fclose(file);

    // Тестуємо однопотокову версію
    start = clock();
    single_thread_file_ops();
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Single-threaded time: %.4f seconds\n", cpu_time_used);

    pthread_mutex_destroy(&file_mutex);
    return 0;
}