
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include "buffer.h"
#include "pierogarnia.h"

#define PRODUCERS 6 // Number of producers
#define CONSUMERS 4 // Number of producers

typedef struct arg_struct arg_struct;

struct arg_struct {
    Buffer *buffer;
    Buffer *buffer_ciasto;
    int index;
};

void *produce(void *arguments)
{
    arg_struct *args = arguments;
    Buffer *buffer = args->buffer;
    int index = args->index;

    int value;
    while (1) {
        sleep(1);
        sem_wait(&buffer->empty);
        pthread_mutex_lock(&buffer->mutex);

        sem_getvalue(&buffer->full, &value);
        printf("Producer %d: Created \"%s\" (%d/%d)\n",
            index, buffer->item_name, value+1, BUFFER_SIZE);

        pthread_mutex_unlock(&buffer->mutex);
        sem_post(&buffer->full);
    }
}

void *consume(void *arguments)
{
    struct arg_struct *args = arguments;
    Buffer *buffer_nadzienie = args->buffer;
    Buffer *buffer_ciasto = args->buffer_ciasto;
    int index = args->index;

    int value_ciasto, value_nadzienie;
    while (1) {
        sleep(1);
        sem_wait(&buffer_nadzienie->full);
        sem_wait(&buffer_ciasto->full);
        pthread_mutex_lock(&buffer_nadzienie->mutex);
        pthread_mutex_lock(&buffer_ciasto->mutex);

        sem_getvalue(&buffer_nadzienie->full, &value_nadzienie);
        sem_getvalue(&buffer_ciasto->full, &value_ciasto);
        printf("Consumer %d: Made pierog with \"%s\" (%d/%d) (Ciasto %d/%d)\n",
            index, buffer_nadzienie->item_name, value_nadzienie, BUFFER_SIZE, value_ciasto, BUFFER_SIZE);
        
        pthread_mutex_unlock(&buffer_nadzienie->mutex);
        pthread_mutex_unlock(&buffer_ciasto->mutex);
        sem_post(&buffer_nadzienie->empty);
        sem_post(&buffer_ciasto->empty);
    }
}

Buffer* get_random_buffer(Pierogarnia *pierogarnia) {
    int index = rand()%4;

    Buffer* random_buffer;

    if (index == 0) {
        random_buffer = &pierogarnia->ciasto;
    }
    else if (index == 1) {
        random_buffer = &pierogarnia->mieso;
    }
    else if (index == 2) {
        random_buffer = &pierogarnia->ser;
    }
    else if (index == 3) {
        random_buffer = &pierogarnia->kapusta;
    }

    return random_buffer;
}

Buffer* get_random_buffer_nadzienie(Pierogarnia *pierogarnia) {
    int index = rand()%3;

    Buffer* random_buffer;

    if (index == 0) {
        random_buffer = &pierogarnia->mieso;
    }
    else if (index == 1) {
        random_buffer = &pierogarnia->ser;
    }
    else if (index == 2) {
        random_buffer = &pierogarnia->kapusta;
    }

    return random_buffer;
}

void fill_buffers(Pierogarnia *pierogarnia, Buffer* buffers[])
{
    // we must have all the ingredients for a pierog
    buffers[0] = &pierogarnia->ciasto;
    buffers[1] = &pierogarnia->mieso;
    buffers[2] = &pierogarnia->ser;
    buffers[3] = &pierogarnia->kapusta;

    // if we have more availabe producers, then we want more "ciasto" produced
    if (PRODUCERS > 4) {
        buffers[4] = &pierogarnia->ciasto;
    }

    // any more available producers will be random
    for (int i=5; i<PRODUCERS; i++) {        
        buffers[i] = get_random_buffer(pierogarnia);
    }
}

void fill_buffers_nadzienie(Pierogarnia *pierogarnia, Buffer* buffers[])
{
    buffers[0] = &pierogarnia->mieso;
    buffers[1] = &pierogarnia->ser;
    buffers[2] = &pierogarnia->kapusta;

    for (int i=3; i<CONSUMERS; i++) {        
        buffers[i] = get_random_buffer_nadzienie(pierogarnia);
    }
}

int main()
{
    srand(time(NULL));

    pthread_t producers[PRODUCERS], consumers[CONSUMERS];

    arg_struct args;
    arg_struct args_list_producers[PRODUCERS], args_list_consumers[CONSUMERS];

    Pierogarnia pierogarnia;
    init_pierogarnia(&pierogarnia);

    args.buffer_ciasto = &pierogarnia.ciasto;

    int max = (CONSUMERS > PRODUCERS ? CONSUMERS : PRODUCERS);
    
    int indexes[max]; // Just used for numbering the producer and consumer
    for (int i = 0; i < max; i++) {
        indexes[i] = i+1;
    }

    Buffer* buffers[PRODUCERS];
    Buffer* buffers_nadzienie[CONSUMERS];
    fill_buffers(&pierogarnia, buffers);
    fill_buffers_nadzienie(&pierogarnia, buffers_nadzienie);

    for (int i = 0; i < PRODUCERS; i++) {
        args.buffer = buffers[i];
        args.index = indexes[i];
        args_list_producers[i] = args;
        pthread_create(&producers[i], NULL, produce, (void *)&args_list_producers[i]);
        sleep(1);
    }
    for (int i = 0; i < CONSUMERS; i++) {
        args.buffer = buffers_nadzienie[i];
        args.index = indexes[i];
        args_list_producers[i] = args;
        pthread_create(&consumers[i], NULL, consume, (void *)&args_list_producers[i]);
        sleep(1);
    }

    for (int i = 0; i < PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    destroy_pierogarnia(&pierogarnia);

    return 0;
}