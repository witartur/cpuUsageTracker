#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "circular_buffer.h"
#include "proc_stat_database.h"
#include "logger.h"

static struct {
    CircularBuffer buffersTable[BufferTypeLast];
} context = {0};

static bool InitSingleBuffer(CircularBuffer *cb, size_t element_size) {
    cb->max_no_of_elements = BUFFER_SIZE;
    cb->no_of_elements = 0;
    cb->size_of_element = element_size;

    cb->buffer = malloc(cb->max_no_of_elements * cb->size_of_element);
    if(cb->buffer == NULL) {
        return false;
    }
    cb->buffer_end = (__uint8_t *)cb->buffer + cb->max_no_of_elements * cb->size_of_element;
    cb->head = cb->buffer;
    cb->tail = cb->buffer;
    return true;
}

bool CB_Init(BufferType buffer_type) {
    switch (buffer_type)
    {
    case BufferTypeReadData:
        InitSingleBuffer(&context.buffersTable[BufferTypeReadData], sizeof(CpuCoreData) * DB_GetCoreNo());
        return true;
    case BufferTypeAnalyzedData:
        InitSingleBuffer(&context.buffersTable[BufferTypeAnalyzedData], sizeof(double) * DB_GetCoreNo());
        return true;
    case BufferTypeLogger:
        InitSingleBuffer(&context.buffersTable[BufferTypeLogger], MAX_MSG_SIZE * sizeof(char));
        return true;
    case BufferTypeLast:
        return false;
    }
}

void CB_Free() {
    if(context.buffersTable[BufferTypeReadData].buffer != NULL)
        free(context.buffersTable[BufferTypeReadData].buffer);

    if(context.buffersTable[BufferTypeAnalyzedData].buffer != NULL)
        free(context.buffersTable[BufferTypeAnalyzedData].buffer);
    
    if(context.buffersTable[BufferTypeLogger].buffer != NULL)
        free(context.buffersTable[BufferTypeLogger].buffer);
}

void CB_PushBack(void *element, BufferType buffer_type) {
    memcpy(context.buffersTable[buffer_type].head, element, context.buffersTable[buffer_type].size_of_element);
    context.buffersTable[buffer_type].head = (__uint8_t*)context.buffersTable[buffer_type].head + context.buffersTable[buffer_type].size_of_element;

    if(context.buffersTable[buffer_type].head == context.buffersTable[buffer_type].buffer_end)
        context.buffersTable[buffer_type].head = context.buffersTable[buffer_type].buffer;

    context.buffersTable[buffer_type].no_of_elements++;
}

void CB_PopFront(void *element, BufferType buffer_type) {
    memcpy(element, context.buffersTable[buffer_type].tail, context.buffersTable[buffer_type].size_of_element);
    context.buffersTable[buffer_type].tail = (__uint8_t*)context.buffersTable[buffer_type].tail + context.buffersTable[buffer_type].size_of_element;

    if(context.buffersTable[buffer_type].tail == context.buffersTable[buffer_type].buffer_end)
        context.buffersTable[buffer_type].tail = context.buffersTable[buffer_type].buffer;

    context.buffersTable[buffer_type].no_of_elements--;
}

// for utests
size_t CB_GetNoOfElements(BufferType buffer_type) {
    return context.buffersTable[buffer_type].no_of_elements;
}
