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

    // printf("No of buffer elements: %lu\n", context.buffersTable[buffer_type].no_of_elements);
    memcpy(context.buffersTable[buffer_type].head, element, context.buffersTable[buffer_type].size_of_element);

    context.buffersTable[buffer_type].head = (__uint8_t*)context.buffersTable[buffer_type].head + context.buffersTable[buffer_type].size_of_element;

    if(context.buffersTable[buffer_type].head == context.buffersTable[buffer_type].buffer_end)
        context.buffersTable[buffer_type].head = context.buffersTable[buffer_type].buffer;

    context.buffersTable[buffer_type].no_of_elements++;

    // printf("No of buffer elements: %lu\n", context.buffersTable[buffer_type].no_of_elements);

    // if(buffer_type == BufferTypeReadData) {
    //     CpuCoreData *element2 = element;
    //     printf("PUSH\nCircullar buffer\nCPU %d - User: %lu, Nice: %lu, System: %lu, Idle: %lu, IO Wait: %lu, IRQ: %lu, Soft IRQ: %lu, Steal: %lu, Guest: %lu, Guest Nice: %lu\n",
    //             element2[0].cpu_id, element2[0].user, element2[0].nice, element2[0].system, element2[0].idle, element2[0].iowait,
    //             element2[0].irq, element2[0].softirq, element2[0].steal, element2[0].guest, element2[0].guest_nice);
    // } else {
    //     double *element3 = element;
    //     printf("PUSH\nCPU:\t\t");
    //     for(int i = 0; i < 9; i++)
    //         printf("%f\t", element3[i]);
    //     printf("\n");
    // }

    return true;
}

bool CB_PopFront(void *element, BufferType buffer_type) {
    if(context.buffersTable[buffer_type].no_of_elements == 0){
        printf("No of elements in buffer is 0\n");
        return false;
    }

    // printf("No of buffer elements: %lu\n", context.buffersTable[buffer_type].no_of_elements);
    memcpy(element, context.buffersTable[buffer_type].tail, context.buffersTable[buffer_type].size_of_element);

    context.buffersTable[buffer_type].tail = (__uint8_t*)context.buffersTable[buffer_type].tail + context.buffersTable[buffer_type].size_of_element;

    if(context.buffersTable[buffer_type].tail == context.buffersTable[buffer_type].buffer_end)
        context.buffersTable[buffer_type].tail = context.buffersTable[buffer_type].buffer;

    context.buffersTable[buffer_type].no_of_elements--;

    // if(buffer_type == BufferTypeReadData) {
    //     CpuCoreData *element2 = element;
    //     printf("POP\nCircullar buffer\nCPU %d - User: %lu, Nice: %lu, System: %lu, Idle: %lu, IO Wait: %lu, IRQ: %lu, Soft IRQ: %lu, Steal: %lu, Guest: %lu, Guest Nice: %lu\n",
    //             element2[0].cpu_id, element2[0].user, element2[0].nice, element2[0].system, element2[0].idle, element2[0].iowait,
    //             element2[0].irq, element2[0].softirq, element2[0].steal, element2[0].guest, element2[0].guest_nice);
    // } else {
    //     double *element3 = element;
    //     printf("POP\nCPU:\t\t");
    //     for(int i = 0; i < 9; i++)
    //         printf("%f\t", element3[i]);
    //     printf("\n");
    // }

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
