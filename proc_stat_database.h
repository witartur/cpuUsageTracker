#ifndef PROC_STAT_DATABASE_H
#define PROC_STAT_DATABASE_H

#include <stdbool.h>

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

void DB_SetCoreNo(unsigned core_no);
unsigned DB_GetCoreNo(); 

bool DB_AddReadProcStatData(CpuCoreData* cpu_data_table);
CpuCoreData* DB_GetReadProcStateData();


#endif  // PROC_STAT_DATABASE_H