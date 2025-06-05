#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_ITERATIONS 5

int shared_counter = 0;
pthread_mutex_t mutex;

void* increment_thread(void* arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);
        int temp = shared_counter;
        temp++;
        sleep(1);  // Імітуємо тривалу операцію
        shared_counter = temp;
        printf("Increment thread: counter = %d\n", shared_counter);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

void* decrement_thread(void* arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);
        int temp = shared_counter;
        temp--;
        sleep(1);  // Імітуємо тривалу операцію
        shared_counter = temp;
        printf("Decrement thread: counter = %d\n", shared_counter);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    
    pthread_mutex_init(&mutex, NULL);
    
    pthread_create(&t1, NULL, increment_thread, NULL);
    pthread_create(&t2, NULL, decrement_thread, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    pthread_mutex_destroy(&mutex);
    
    printf("Final counter value: %d\n", shared_counter);
    return 0;
}
