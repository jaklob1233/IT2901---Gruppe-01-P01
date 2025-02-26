/*
 * Company:    THALES Communications & Security
 *
 * Copyright   2023 - Gennevillliers - THALES Communications & Security
 *
 * All rights especially the right for copying and distribution as
 * well as translation reserved.
 * No part of the product shall be reproduced or stored, processed
 * copied or distributed with electronic tools or by paper copy or
 * any other process without written authorization of
 * THALES Communications & Security
 */
#include "fifo.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

fifo_t* fifo_new(size_t size) {
    fifo_t* f = (fifo_t*)malloc(sizeof(fifo_t));
    fifo_init(f, size);
    return f;
}

void fifo_free(fifo_t* self) {
    fifo_uninit(self);
    free(self);
}

int fifo_init(fifo_t* self, size_t size) {
    self->buffer = (char*)malloc(size);
    self->count  = 0;
    self->size   = size;
    return 0;
}

void fifo_uninit(fifo_t* self) {
    free(self->buffer);
}

int fifo_clear(fifo_t* self) {
    self->count = 0;
    return 0;
}

int fifo_push(fifo_t* self, const void* item, size_t size) {
    if (self->count + (int)size > (int)self->size)
        return -1;

    memcpy(&self->buffer[self->count], item, size);
    self->count += (int)size;
    return 0;
}

int fifo_pop(fifo_t* self, void* item, size_t size) {
    if (self->count - (int)size < 0)
        return -1;

    memcpy(item, self->buffer, size);
    memmove(self->buffer, &self->buffer[size], (size_t)self->count-size);
    self->count -= (int)size;
    return 0;
}

