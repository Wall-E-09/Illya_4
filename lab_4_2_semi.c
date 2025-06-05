#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define FILENAME "shared_file.txt"

sem_t *writer_sem, *reader_sem;
pthread_mutex_t file_mutex;

void *writer_with_sem(void *arg) {
    int fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return NULL;
    }
    
    for (int i = 0; i < 5; i++) {
        sem_wait(writer_sem);
        
        dprintf(fd, "Write %d (with sem)\n", i);
        printf("Writer wrote with sem: %d\n", i);
        
        sem_post(reader_sem);
        usleep(100000);
    }
    
    close(fd);
    return NULL;
}

void *reader_with_sem(void *arg) {
    int fd = open(FILENAME, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return NULL;
    }
    
    char buffer[256];
    ssize_t bytes_read;
    
    for (int i = 0; i < 5; i++) {
        sem_wait(reader_sem);
        
        lseek(fd, 0, SEEK_SET);
        bytes_read = read(fd, buffer, sizeof(buffer)-1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Reader read with sem: %s", buffer);
        }
        
        sem_post(writer_sem);
        usleep(100000);
    }
    
    close(fd);
    return NULL;
}

void *writer_without_sem(void *arg) {
    int fd = open("file_without_sem.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return NULL;
    }
    
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&file_mutex);
        
        dprintf(fd, "Write %d (no sem)\n", i);
        printf("Writer wrote without sem: %d\n", i);
        
        pthread_mutex_unlock(&file_mutex);
        usleep(100000);
    }
    
    close(fd);
    return NULL;
}

void *reader_without_sem(void *arg) {
    int fd = open("file_without_sem.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return NULL;
    }
    
    char buffer[256];
    ssize_t bytes_read;
    
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&file_mutex);
        
        lseek(fd, 0, SEEK_SET);
        bytes_read = read(fd, buffer, sizeof(buffer)-1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Reader read without sem: %s", buffer);
        }
        
        pthread_mutex_unlock(&file_mutex);
        usleep(100000);
    }
    
    close(fd);
    return NULL;
}

int main() {
    pthread_t writer_thread, reader_thread;
    pthread_t writer_no_sem_thread, reader_no_sem_thread;
    
    writer_sem = sem_open("/writer_sem", O_CREAT, 0644, 1);
    reader_sem = sem_open("/reader_sem", O_CREAT, 0644, 0);
    pthread_mutex_init(&file_mutex, NULL);
    
    if (writer_sem == SEM_FAILED || reader_sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }
    
    printf("\n=== Робота з семафорами ===\n");
    pthread_create(&writer_thread, NULL, writer_with_sem, NULL);
    pthread_create(&reader_thread, NULL, reader_with_sem, NULL);
    
    pthread_join(writer_thread, NULL);
    pthread_join(reader_thread, NULL);
    
    printf("\n=== Робота без семафорів (тільки м'ютекси) ===\n");
    pthread_create(&writer_no_sem_thread, NULL, writer_without_sem, NULL);
    pthread_create(&reader_no_sem_thread, NULL, reader_without_sem, NULL);
    
    pthread_join(writer_no_sem_thread, NULL);
    pthread_join(reader_no_sem_thread, NULL);
    
    sem_close(writer_sem);
    sem_close(reader_sem);
    sem_unlink("/writer_sem");
    sem_unlink("/reader_sem");
    pthread_mutex_destroy(&file_mutex);
    
    return 0;
}
