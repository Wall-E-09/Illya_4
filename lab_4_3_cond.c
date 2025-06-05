#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int condition = 0;

void* worker_thread(void* arg) {
    printf("Worker thread started\n");
    
    pthread_mutex_lock(&mutex);
    while (condition == 0) {
        printf("Worker waiting for condition...\n");
        pthread_cond_wait(&cond, &mutex);
    }
    printf("Worker condition met!\n");
    pthread_mutex_unlock(&mutex);
    
    return NULL;
}

int main() {
    pthread_t worker;
    
    pthread_create(&worker, NULL, worker_thread, NULL);
    
    sleep(3);  // Імітуємо роботу основного потоку
    
    pthread_mutex_lock(&mutex);
    condition = 1;
    printf("Main thread signaling condition\n");
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    
    pthread_join(worker, NULL);
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    
    return 0;
}
