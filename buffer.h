#ifndef BUFFER_H
#define BUFFER_H

#include <semaphore.h>
#include <pthread.h>
#include <string.h>

#define BUFFER_SIZE 5 // Size of the buffer

typedef struct Buffer Buffer;

struct Buffer {
    char* item_name;
    sem_t empty;
    sem_t full;
    pthread_mutex_t mutex;
};

void init_buffer(Buffer *buffer, char* name) {
    buffer->item_name = name;
    pthread_mutex_init(&buffer->mutex, NULL);
    sem_init(&buffer->empty, 0, BUFFER_SIZE);
    sem_init(&buffer->full, 0, 0);
}

void destroy_buffor(Buffer *buffer) {
    pthread_mutex_destroy(&buffer->mutex);
    sem_destroy(&buffer->empty);
    sem_destroy(&buffer->full);
}

#endif