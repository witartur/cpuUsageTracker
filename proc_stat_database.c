#include <stdlib.h>
#include <stdbool.h>

#include "proc_stat_database.h"
#include "circular_buffer.h"
#include "reader.h"


static struct
{
    unsigned cores_no;
} context = {0};

bool DB_Init() {
    context.cores_no = ReadCoresNo();
}

void DB_SetCoreNo(unsigned core_no) {
    if(core_no < 1)
        return;
    
    context.cores_no = core_no;
}

unsigned DB_GetCoreNo() {
    return context.cores_no;
}

bool DB_AddReadProcStatData(CpuCoreData* cpu_data_table) {
    if(context.cores_no < 1)
        return false;

    CB_PushBack(cpu_data_table, BufferTypeReadData);
    return true;
}

CpuCoreData* DB_GetReadProcStateData() {
    if(context.cores_no < 1)
        return NULL;

    CpuCoreData *cpu_data_table;
    CB_PopFront(cpu_data_table, BufferTypeReadData);
    return cpu_data_table;
}

