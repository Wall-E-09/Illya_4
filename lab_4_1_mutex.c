#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 2
#define INCREMENTS_PER_THREAD 100000

int shared_counter_with_mutex = 0;
int shared_counter_without_mutex = 0;
pthread_mutex_t mutex;

void *increment_with_mutex(void *arg) {
    for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
        pthread_mutex_lock(&mutex);
        shared_counter_with_mutex++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *increment_without_mutex(void *arg) {
    for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
        shared_counter_without_mutex++;
    }
    return NULL;
}

int main() {
    pthread_t threads_with_mutex[NUM_THREADS];
    pthread_t threads_without_mutex[NUM_THREADS];
    
    pthread_mutex_init(&mutex, NULL);
    
    // З м'ютексом
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads_with_mutex[i], NULL, increment_with_mutex, NULL);
    }
    
    // Без м'ютекса
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads_without_mutex[i], NULL, increment_without_mutex, NULL);
    }
    
    // Очікуємо завершення потоків з м'ютексом
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads_with_mutex[i], NULL);
    }
    
    // Очікуємо завершення потоків без м'ютекса
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads_without_mutex[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex);
    
    printf("\n=== Результати роботи з м'ютексом та без ===\n");
    printf("Очікуване значення лічильника: %d\n", NUM_THREADS * INCREMENTS_PER_THREAD);
    printf("З м'ютексом:    %d\n", shared_counter_with_mutex);
    printf("Без м'ютекса:   %d\n", shared_counter_without_mutex);
    
    return 0;
}
