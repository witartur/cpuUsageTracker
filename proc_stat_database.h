#ifndef PROC_STAT_DATABASE_H
#define PROC_STAT_DATABASE_H

typedef struct
{
    unsigned cpu_id;
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


void DB_SetCoreNo(unsigned core_no);
unsigned DB_GetCoreNo(); 
void DB_CpuDataBufferAllocation();

#endif