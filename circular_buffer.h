#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdlib.h>

typedef struct {
    void *buffer;
    void *buffer_end;
    size_t capacity;
    size_t count;
    size_t sz;
    void *head;
    void *tail;
} CircularBuffer;



#endif