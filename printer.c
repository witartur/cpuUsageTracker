#include <stdio.h>

#include "printer.h"
#include "proc_stat_database.h"


void PrintRawData(CpuCoreData *dataToPrint) {
    printf("\n");
    
    for (int i = 0; i < DB_GetCoreNo(); i++)
    {
        printf("cpu%d,   %ld\n", dataToPrint[i].cpu_id, dataToPrint[i].user);
    }
    
}