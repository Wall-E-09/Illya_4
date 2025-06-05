#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define BUFFER_SIZE 10
#define NUM_ITEMS 10000

int buffer[BUFFER_SIZE];
int count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_producer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_consumer = PTHREAD_COND_INITIALIZER;

// Багатопотокова версія з синхронізацією
void* producer_sync(void* arg) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        pthread_mutex_lock(&mutex);
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&cond_producer, &mutex);
        }
        buffer[count++] = i;
        pthread_cond_signal(&cond_consumer);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* consumer_sync(void* arg) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        pthread_mutex_lock(&mutex);
        while (count == 0) {
            pthread_cond_wait(&cond_consumer, &mutex);
        }
        int item = buffer[--count];
        pthread_cond_signal(&cond_producer);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

// Однопотокова версія
void single_thread_buffer() {
    for (int i = 0; i < NUM_ITEMS; i++) {
        while (count == BUFFER_SIZE) {} // Очікування
        buffer[count++] = i;
        
        while (count == 0) {} // Очікування
        int item = buffer[--count];
    }
}

int main() {
    clock_t start, end;
    double cpu_time_used;
    pthread_t prod, cons;

    // Тест 1: Багатопотокова з синхронізацією
    count = 0;
    start = clock();
    
    pthread_create(&prod, NULL, producer_sync, NULL);
    pthread_create(&cons, NULL, consumer_sync, NULL);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Multithreaded sync: %.4f seconds\n", cpu_time_used);

    // Тест 2: Однопотокова версія
    count = 0;
    start = clock();
    single_thread_buffer();
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Single-threaded: %.4f seconds\n", cpu_time_used);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_producer);
    pthread_cond_destroy(&cond_consumer);

    return 0;
}