#ifndef PROC_STAT_DATABASE_H
#define PROC_STAT_DATABASE_H

#include <stdbool.h>

#define BUFFER_SIZE 5
typedef enum {
    BufferTypeReadData = 0,
    BufferTypeAnalyzedData,
    BufferTypeLogger,
    BufferTypeLast,
} BufferType;

typedef struct {
    int cpu_id;
    int padding0;
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

bool DB_Init(void);
void DB_DeInit(void);

unsigned DB_GetCoreNo(void);

void DB_AddDataToReadDataBuffer(CpuCoreData *data);
void DB_AddDataToAnalyzedDataBuffer(double *data);

void DB_GetDataFromReadDataBuffer(void* destination);
void DB_GetDataFromAnalyzedDataBuffer(void* destination);

#endif  // PROC_STAT_DATABASE_H
