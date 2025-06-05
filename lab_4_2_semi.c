#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_OPERATIONS 1000000

int shared_counter = 0;
sem_t counter_sem;

// Багатопотокова версія з синхронізацією
void* incrementer_sync(void* arg) {
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        sem_wait(&counter_sem);
        shared_counter++;
        sem_post(&counter_sem);
    }
    return NULL;
}

void* decrementer_sync(void* arg) {
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        sem_wait(&counter_sem);
        shared_counter--;
        sem_post(&counter_sem);
    }
    return NULL;
}

// Багатопотокова версія без синхронізації
void* incrementer_unsync(void* arg) {
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        shared_counter++;
    }
    return NULL;
}

void* decrementer_unsync(void* arg) {
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        shared_counter--;
    }
    return NULL;
}

// Однопотокова версія
void single_thread_counter() {
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        shared_counter++;
    }
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        shared_counter--;
    }
}

int main() {
    clock_t start, end;
    double cpu_time_used;
    pthread_t t1, t2;

    // Тест 1: Багатопотокова з синхронізацією
    sem_init(&counter_sem, 0, 1);
    shared_counter = 0;
    start = clock();
    
    pthread_create(&t1, NULL, incrementer_sync, NULL);
    pthread_create(&t2, NULL, decrementer_sync, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Multithreaded sync: %.4f sec, counter=%d (correct)\n", cpu_time_used, shared_counter);
    sem_destroy(&counter_sem);

    // Тест 2: Багатопотокова без синхронізації
    shared_counter = 0;
    start = clock();
    
    pthread_create(&t1, NULL, incrementer_unsync, NULL);
    pthread_create(&t2, NULL, decrementer_unsync, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Multithreaded unsync: %.4f sec, counter=%d (incorrect)\n", cpu_time_used, shared_counter);

    // Тест 3: Однопотокова версія
    shared_counter = 0;
    start = clock();
    single_thread_counter();
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Single-threaded: %.4f sec, counter=%d (correct)\n", cpu_time_used, shared_counter);

    return 0;
}