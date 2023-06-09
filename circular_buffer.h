#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdlib.h>
#include "proc_stat_database.h"

typedef struct {
    BufferType name;
    int padding0;
    void *buffer;
    void *buffer_end;
    size_t max_no_of_elements;
    size_t no_of_elements;
    size_t size_of_element;
    void *head;
    void *tail;
} CircularBuffer;

bool CB_Init(BufferType buffer_type);
void CB_Free(void);
void CB_PushBack(void *element, BufferType buffer_type);
void CB_PopFront(void *element, BufferType buffer_type);

// for utests
size_t CB_GetNoOfElements(BufferType buffer_type);

#endif  // CIRCULAR_BUFFER_H
