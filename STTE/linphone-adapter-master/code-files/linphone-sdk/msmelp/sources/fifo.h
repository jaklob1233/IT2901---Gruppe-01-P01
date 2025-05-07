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
#ifndef FIFO_H
#define FIFO_H

#include <stddef.h>

typedef struct fifo_s {
    char*  buffer;
    int    count;
    size_t size;
} fifo_t;

fifo_t* fifo_new(size_t size);
void    fifo_free(fifo_t* self);

int  fifo_init(fifo_t* self, size_t size);
void fifo_uninit(fifo_t* self);

int fifo_clear(fifo_t* self);
int fifo_push(fifo_t* self, const void* item, size_t size);
int fifo_pop(fifo_t* self, void* item, size_t size);

#define fifo_is_empty(self)     ((self)->count == 0)
#define fifo_is_full(self)      ((self)->count == (int)(self)->size)
#define fifo_count(self)        ((self)->count)

#endif

