#include <string.h>

#include "circular_buffer.h"




void CB_Init(CircularBuffer *cb, size_t capacity, size_t sz) {
    cb->buffer = malloc(capacity * sz);
    if(cb->buffer == NULL){

    }


    cb->buffer_end = (char *)cb->buffer + capacity * sz;
    cb->capacity = capacity;
    cb->count = 0;
    cb->sz = sz;
    cb->head = cb->buffer;
    cb->tail = cb->buffer;
}

void CB_Free(CircularBuffer *cb)
{
    free(cb->buffer);
}

void CB_PushBack(CircularBuffer *cb, const void *item)
{
    if(cb->count == cb->capacity){
    }
    memcpy(cb->head, item, cb->sz);
    cb->head = (char*)cb->head + cb->sz;
    if(cb->head == cb->buffer_end)
        cb->head = cb->buffer;
    cb->count++;
}

void CB_PopFront(CircularBuffer *cb, void *item)
{
    if(cb->count == 0){

    }
    memcpy(item, cb->tail, cb->sz);
    cb->tail = (char*)cb->tail + cb->sz;
    if(cb->tail == cb->buffer_end)
        cb->tail = cb->buffer;
    cb->count--;
}