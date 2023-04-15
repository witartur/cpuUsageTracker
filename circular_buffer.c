#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "circular_buffer.h"
#include "proc_stat_database.h"

#define BUFFER_SIZE 5

static struct {
    CircularBuffer buffersTable[BufferTypeLast];
} context = {0};

static bool InitSingleBuffer(CircularBuffer *cb) {
    cb->max_no_of_elements = BUFFER_SIZE;
    cb->no_of_elements = 0;
    cb->size_of_element = sizeof(CpuCoreData) * DB_GetCoreNo();

    cb->buffer = malloc(cb->max_no_of_elements * cb->size_of_element);
    if(cb->buffer == NULL){
        printf("CANNOT ALLOCATE BUFFER MEMORY");
        return false;
    }
    cb->buffer_end = (char *)cb->buffer + cb->max_no_of_elements * cb->size_of_element;
    cb->head = cb->buffer;
    cb->tail = cb->buffer;
    return true;
}

bool CB_Init() {
    return InitSingleBuffer(&context.buffersTable[BufferTypeReadData])
        && InitSingleBuffer(&context.buffersTable[BufferTypeAnalisedData]);
}

void CB_Free() {
    free(context.buffersTable[BufferTypeReadData].buffer);
    free(context.buffersTable[BufferTypeAnalisedData].buffer);
}

bool CB_PushBack(CpuCoreData *element, BufferType buffer_type) {
    if(context.buffersTable[buffer_type].no_of_elements == context.buffersTable[buffer_type].max_no_of_elements){
        printf("Buffer is full\n");
        return false;
    }

    printf("No of buffer elements: %lu\n", context.buffersTable[buffer_type].no_of_elements);
    memcpy(context.buffersTable[buffer_type].head, element, context.buffersTable[buffer_type].size_of_element);
    context.buffersTable[buffer_type].head = (CpuCoreData*)context.buffersTable[buffer_type].head + context.buffersTable[buffer_type].size_of_element;

    if(context.buffersTable[buffer_type].head == context.buffersTable[buffer_type].buffer_end)
        context.buffersTable[buffer_type].head = context.buffersTable[buffer_type].buffer;

    context.buffersTable[buffer_type].no_of_elements++;

    printf("No of buffer elements: %lu\n", context.buffersTable[buffer_type].no_of_elements);
    return true;
}

bool CB_PopFront(CpuCoreData *element, BufferType buffer_type) {
    if(context.buffersTable[buffer_type].no_of_elements == 0){
        printf("No of elements in buffer is 0\n");
        return false;
    }

    printf("No of buffer elements: %lu\n", context.buffersTable[buffer_type].no_of_elements);
    memcpy(element, context.buffersTable[buffer_type].tail, context.buffersTable[buffer_type].size_of_element);

    context.buffersTable[buffer_type].tail = (CpuCoreData*)context.buffersTable[buffer_type].tail + context.buffersTable[buffer_type].size_of_element;

    if(context.buffersTable[buffer_type].tail == context.buffersTable[buffer_type].buffer_end)
        context.buffersTable[buffer_type].tail = context.buffersTable[buffer_type].buffer;

    context.buffersTable[buffer_type].no_of_elements--;
    return true;
}

void CB_Print() {
    int core_no = DB_GetCoreNo();
    CpuCoreData *element = malloc(sizeof(CpuCoreData)* core_no);
    int i, j;
    unsigned elements_to_print_no = context.buffersTable[BufferTypeReadData].no_of_elements;
    printf("Printing Circular Buffer:\n");
    printf("No of buffer elements: %u\n", elements_to_print_no);
    printf("Core No: %d\n", core_no);

    for (i = 0; i < elements_to_print_no; i++) {
        printf("Pop result %d\n", CB_PopFront(element, BufferTypeReadData));

        for (j = 0; j < core_no; j++)
        {
            printf("CPU %d - User: %lu, Nice: %lu, System: %lu, Idle: %lu, IO Wait: %lu, IRQ: %lu, Soft IRQ: %lu, Steal: %lu, Guest: %lu, Guest Nice: %lu\n",
               element[j].cpu_id, element[j].user, element[j].nice, element[j].system, element[j].idle, element[j].iowait,
               element[j].irq, element[j].softirq, element[j].steal, element[j].guest, element[j].guest_nice);
        }
    }
    free(element);
}
