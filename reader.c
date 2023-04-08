#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#include "reader.h"
#include "proc_stat_database.h"

static struct
{
    unsigned core_no;
} context;

static bool IsFileEmpty(FILE *file, char *read_line, size_t length) {
    if (fgets(read_line, length, file) == NULL)
        return true;
    
    return false;
}

static bool IsLineCpuCoreInfo(char *read_line) {
    if (strncmp(read_line, "cpu", 3) == 0)
        return true;
    
    return false;
}

static unsigned CountCores() {
    FILE *file = fopen("/proc/stat", "r");
    char read_cpu[5];
    unsigned core_no = 0;

    while(1)
    {
        if(IsFileEmpty(file, read_cpu, sizeof(read_cpu)))
            break;
        
        if(IsLineCpuCoreInfo(read_cpu) == false)
            break;

        core_no++;
    }

    fclose(file);

    context.core_no = core_no;
    return core_no;
}

static bool ReadProcStatFromFile() {
    FILE *file = fopen("/proc/stat", "r");
    char read_line[512];
    CpuCoreData temp_array[context.core_no];
    unsigned core_index = 0;

    while(1)
    {
        if (IsFileEmpty(file, read_line, sizeof(read_line)))
            return false;
        
        if (IsLineCpuCoreInfo(read_line))
        {
            sscanf(read_line, "%u %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
                &temp_array[core_index].cpu_id, &temp_array[core_index].user, &temp_array[core_index].nice,
                &temp_array[core_index].system, &temp_array[core_index].idle, &temp_array[core_index].iowait,
                &temp_array[core_index].irq, &temp_array[core_index].softirq, &temp_array[core_index].steal,
                &temp_array[core_index].guest, &temp_array[core_index].guest_nice);     
        }
        
        core_index++;
    }

    fclose(file);
    return true;  
}

