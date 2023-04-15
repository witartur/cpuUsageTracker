#ifndef PROC_STAT_DATABASE_H
#define PROC_STAT_DATABASE_H

#include <stdbool.h>

typedef enum {
    BufferTypeReadData = 0,
    BufferTypeAnalyzedData,
    BufferTypeLast,
} BufferType;

typedef struct
{
    int cpu_id;
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
    unsigned long iowait;
    unsigned long irq;
    unsigned long softirq;
    unsigned long steal;
    unsigned long guest;
    unsigned long guest_nice;
} CpuCoreData;

bool DB_Init();
unsigned DB_GetCoreNo();

bool DB_AddDataToBuffer(void* data, BufferType buffer_type);
bool DB_GetDataFromBuffer(void* destination, BufferType buffer_type);

#endif  // PROC_STAT_DATABASE_H