#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 5

// З умовними змінними
int buffer_with_cv[BUFFER_SIZE];
int count_with_cv = 0;
int in_with_cv = 0;
int out_with_cv = 0;
pthread_mutex_t mutex_with_cv = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_producer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_consumer = PTHREAD_COND_INITIALIZER;

// Без умовних змінних
int buffer_without_cv[BUFFER_SIZE];
int count_without_cv = 0;
int in_without_cv = 0;
int out_without_cv = 0;
pthread_mutex_t mutex_without_cv = PTHREAD_MUTEX_INITIALIZER;

void *producer_with_cv(void *arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex_with_cv);
        
        while (count_with_cv == BUFFER_SIZE) {
            pthread_cond_wait(&cond_producer, &mutex_with_cv);
        }
        
        buffer_with_cv[in_with_cv] = i;
        printf("Produced with CV: %d\n", i);
        in_with_cv = (in_with_cv + 1) % BUFFER_SIZE;
        count_with_cv++;
        
        pthread_cond_signal(&cond_consumer);
        pthread_mutex_unlock(&mutex_with_cv);
        
        usleep(rand() % 100000);
    }
    return NULL;
}

void *consumer_with_cv(void *arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex_with_cv);
        
        while (count_with_cv == 0) {
            pthread_cond_wait(&cond_consumer, &mutex_with_cv);
        }
        
        int item = buffer_with_cv[out_with_cv];
        printf("Consumed with CV: %d\n", item);
        out_with_cv = (out_with_cv + 1) % BUFFER_SIZE;
        count_with_cv--;
        
        pthread_cond_signal(&cond_producer);
        pthread_mutex_unlock(&mutex_with_cv);
        
        usleep(rand() % 200000);
    }
    return NULL;
}

void *producer_without_cv(void *arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex_without_cv);
        
        if (count_without_cv == BUFFER_SIZE) {
            pthread_mutex_unlock(&mutex_without_cv);
            usleep(10000); // Замість очікування на умовну змінну
            continue;
        }
        
        buffer_without_cv[in_without_cv] = i;
        printf("Produced without CV: %d\n", i);
        in_without_cv = (in_without_cv + 1) % BUFFER_SIZE;
        count_without_cv++;
        
        pthread_mutex_unlock(&mutex_without_cv);
        
        usleep(rand() % 100000);
    }
    return NULL;
}

void *consumer_without_cv(void *arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex_without_cv);
        
        if (count_without_cv == 0) {
            pthread_mutex_unlock(&mutex_without_cv);
            usleep(10000); // Замість очікування на умовну змінну
            continue;
        }
        
        int item = buffer_without_cv[out_without_cv];
        printf("Consumed without CV: %d\n", item);
        out_without_cv = (out_without_cv + 1) % BUFFER_SIZE;
        count_without_cv--;
        
        pthread_mutex_unlock(&mutex_without_cv);
        
        usleep(rand() % 200000);
    }
    return NULL;
}

int main() {
    pthread_t producer_cv_thread, consumer_cv_thread;
    pthread_t producer_no_cv_thread, consumer_no_cv_thread;
    
    printf("\n=== Робота з умовними змінними ===\n");
    pthread_create(&producer_cv_thread, NULL, producer_with_cv, NULL);
    pthread_create(&consumer_cv_thread, NULL, consumer_with_cv, NULL);
    
    printf("\n=== Робота без умовних змінних ===\n");
    pthread_create(&producer_no_cv_thread, NULL, producer_without_cv, NULL);
    pthread_create(&consumer_no_cv_thread, NULL, consumer_without_cv, NULL);
    
    pthread_join(producer_cv_thread, NULL);
    pthread_join(consumer_cv_thread, NULL);
    pthread_join(producer_no_cv_thread, NULL);
    pthread_join(consumer_no_cv_thread, NULL);
    
    return 0;
}
