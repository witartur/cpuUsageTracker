#ifndef PROC_STAT_DATABASE_H
#define PROC_STAT_DATABASE_H

#include <stdbool.h>

#define BUFFER_SIZE 5
typedef enum {
    BufferTypeReadData = 0,
    BufferTypeAnalyzedData,
    BufferTypeLast,
} BufferType;

typedef struct {
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
bool DB_Denit();
unsigned DB_GetCoreNo();
bool DB_AddDataToReadDataBuffer(CpuCoreData *data);
bool DB_AddDataToAnalyzedDataBuffer(double *data);
bool DB_GetDataFromReadDataBuffer(void* destination);
bool DB_GetDataFromAnalyzedDataBuffer(void* destination);

#endif  // PROC_STAT_DATABASE_H
