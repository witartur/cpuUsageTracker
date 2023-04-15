#include <stdlib.h>
#include <stdbool.h>

#include "proc_stat_database.h"


static struct
{
    unsigned cores_no;
    CpuCoreData *cpuAllCoresData;

} context = {0};

void DB_SetCoreNo(unsigned core_no){
    if(core_no < 1)
        return;
    
    context.cores_no = core_no;
}

unsigned DB_GetCoreNo(){
    return context.cores_no;
}

// void DB_CpuDataBufferAllocation() {
//     context.cpuAllCoresData = malloc(sizeof(CpuCoreData) * context.cores_no * BUFFER_SIZE);
// }
