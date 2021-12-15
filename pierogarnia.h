#ifndef PIEROGARNIA_H
#define PIEROGARNIA_H

#include <semaphore.h>
#include <pthread.h>
#include "buffer.h"

typedef struct Pierogarnia Pierogarnia;

struct Pierogarnia {
    Buffer ciasto;
    Buffer mieso;
    Buffer kapusta;
    Buffer ser;
};

void init_pierogarnia(Pierogarnia *pierogarnia) {
    Buffer buff_kapusta, buff_ser, buff_mieso, buff_ciasto;
    init_buffer(&buff_kapusta, "Kapusta");
    init_buffer(&buff_ser, "Ser");
    init_buffer(&buff_mieso, "Mieso");
    init_buffer(&buff_ciasto, "Ciasto");

    pierogarnia->ciasto=buff_ciasto;
    pierogarnia->mieso=buff_mieso;
    pierogarnia->kapusta=buff_kapusta;
    pierogarnia->ser=buff_ser;
}

void destroy_pierogarnia(Pierogarnia *pierogarnia) {
    destroy_buffor(&pierogarnia->ciasto);
    destroy_buffor(&pierogarnia->mieso);
    destroy_buffor(&pierogarnia->kapusta);
    destroy_buffor(&pierogarnia->ser);
}

#endif