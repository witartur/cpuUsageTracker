#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "circular_buffer.h"
#include "proc_stat_database.h"

#define BUFFER_SIZE 5

static struct {
    CircularBuffer buffersTable[BufferTypeLast];
} context = {0};

static bool InitSingleBuffer(CircularBuffer *cb, BufferType buffer_type) {
    cb->max_no_of_elements = BUFFER_SIZE;
    cb->no_of_elements = 0;

    if(buffer_type == BufferTypeReadData)
        cb->size_of_element = sizeof(CpuCoreData) * DB_GetCoreNo();
    else
        cb->size_of_element = sizeof(double) * DB_GetCoreNo();

    cb->buffer = malloc(cb->max_no_of_elements * cb->size_of_element);
    if(cb->buffer == NULL){
        printf("CANNOT ALLOCATE BUFFER MEMORY");
        return false;
    }
    cb->buffer_end = (__uint8_t *)cb->buffer + cb->max_no_of_elements * cb->size_of_element;
    cb->head = cb->buffer;
    cb->tail = cb->buffer;
    return true;
}

bool CB_Init() {
    return InitSingleBuffer(&context.buffersTable[BufferTypeReadData], BufferTypeReadData)
        && InitSingleBuffer(&context.buffersTable[BufferTypeAnalyzedData], BufferTypeAnalyzedData);
}

void CB_Free() {
    free(context.buffersTable[BufferTypeReadData].buffer);
    free(context.buffersTable[BufferTypeAnalyzedData].buffer);
}

bool CB_PushBack(void *element, BufferType buffer_type) {
    if(context.buffersTable[buffer_type].no_of_elements == context.buffersTable[buffer_type].max_no_of_elements){
        printf("Buffer is full\n");
        return false;
    }

    memcpy(context.buffersTable[buffer_type].head, element, context.buffersTable[buffer_type].size_of_element);
    context.buffersTable[buffer_type].head = (__uint8_t*)context.buffersTable[buffer_type].head + context.buffersTable[buffer_type].size_of_element;

    if(context.buffersTable[buffer_type].head == context.buffersTable[buffer_type].buffer_end)
        context.buffersTable[buffer_type].head = context.buffersTable[buffer_type].buffer;

    context.buffersTable[buffer_type].no_of_elements++;

    return true;
}

bool CB_PopFront(void *element, BufferType buffer_type) {
    if(context.buffersTable[buffer_type].no_of_elements == 0){
        printf("No of elements in buffer is 0\n");
        return false;
    }

    memcpy(element, context.buffersTable[buffer_type].tail, context.buffersTable[buffer_type].size_of_element);
    context.buffersTable[buffer_type].tail = (__uint8_t*)context.buffersTable[buffer_type].tail + context.buffersTable[buffer_type].size_of_element;

    if(context.buffersTable[buffer_type].tail == context.buffersTable[buffer_type].buffer_end)
        context.buffersTable[buffer_type].tail = context.buffersTable[buffer_type].buffer;

    context.buffersTable[buffer_type].no_of_elements--;

    return true;
}
