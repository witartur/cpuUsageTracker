#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#include "reader.h"
#include "proc_stat_database.h"
#include "printer.h"
#include "circular_buffer.h"

#define BUFFER_READ 1
static struct
{
    unsigned core_no;
} context;

static bool IsEndOfFile(FILE *file, char *read_line, size_t length) {
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

    if(file == NULL) {
        printf("Can't open file\n");
        return 0;
    }

    char read_cpu[100];
    unsigned core_no = 0;

    while(1)
    {
        if(IsEndOfFile(file, read_cpu, sizeof(read_cpu))) {
            printf("End of file\n");
            break;
        }

        if(IsLineCpuCoreInfo(read_cpu) == false)
            break;

        core_no++;
    }
    fclose(file);

    context.core_no = core_no;
    printf("Core no: %d\n", core_no);
    return core_no;
}

bool Reader_Init() {
    unsigned core_no = CountCores();

    if(core_no > 0)
        return true;
    return false;
}

unsigned ReadCoresNo() {
    return context.core_no;
}

bool ReadProcStatFromFile() {

    printf("ReadProcStatFromFile begin\n\n");
    FILE *file = fopen("/proc/stat", "r");
    if (file == NULL)
    {
        printf("File does not exist\n");
    }

    char read_line[512];
    CpuCoreData temp_array[context.core_no];
    int core_index = -1;

    while(1) {
        if (IsEndOfFile(file, read_line, sizeof(read_line))) {
            printf("EOF\n");
            fclose(file);
            return false;
        }

        if(IsLineCpuCoreInfo(read_line) == false)
            break;

        int index = core_index + 1;
        temp_array[index].cpu_id = core_index;
        char bin[6];

        int result = sscanf(read_line, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
            bin,
            &temp_array[index].user,
            &temp_array[index].nice,
            &temp_array[index].system,
            &temp_array[index].idle,
            &temp_array[index].iowait,
            &temp_array[index].irq,
            &temp_array[index].softirq,
            &temp_array[index].steal,
            &temp_array[index].guest,
            &temp_array[index].guest_nice);

        printf("---------Read proc stat data---------\n");
        printf("CPU line index %d\n", core_index);
        printf("cpu_id:  %d\n", temp_array[index].cpu_id);
        printf("bin:  %s\n", bin);
        printf("user: %lu\n\n", temp_array[index].user);
        core_index++;
    }

    printf("End of whileloop\n");
    DB_AddDataToBuffer(temp_array, BufferTypeReadData);

    fclose(file);
    return true;
}

