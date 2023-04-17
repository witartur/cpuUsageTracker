#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "analyzer.h"
#include "proc_stat_database.h"

static struct {
    unsigned core_no;
    CpuCoreData *previous_cpu_each_core_data;
} context = {0};

bool Analyzer_Init() {
    context.core_no = DB_GetCoreNo();
    context.previous_cpu_each_core_data = malloc(sizeof(CpuCoreData) * context.core_no);

    if (context.previous_cpu_each_core_data == NULL) {
        return false;
    }
    
    memset(context.previous_cpu_each_core_data, 0, sizeof(CpuCoreData) * context.core_no);
    return true;
}

static double Calculate(CpuCoreData prev_element, CpuCoreData new_element) {
    // printf("Prev element\nCPU %d - User: %lu, Nice: %lu, System: %lu, Idle: %lu, IO Wait: %lu, IRQ: %lu, Soft IRQ: %lu, Steal: %lu, Guest: %lu, Guest Nice: %lu\n",
    //     prev_element.cpu_id, prev_element.user, prev_element.nice, prev_element.system, prev_element.idle, prev_element.iowait,
    //     prev_element.irq, prev_element.softirq, prev_element.steal, prev_element.guest, prev_element.guest_nice);
    
    // printf("New element\nCPU %d - User: %lu, Nice: %lu, System: %lu, Idle: %lu, IO Wait: %lu, IRQ: %lu, Soft IRQ: %lu, Steal: %lu, Guest: %lu, Guest Nice: %lu\n",
    //     new_element.cpu_id, new_element.user, new_element.nice, new_element.system, new_element.idle, new_element.iowait,
    //     new_element.irq, new_element.softirq, new_element.steal, new_element.guest, new_element.guest_nice);


    unsigned long prev_idle = prev_element.idle + prev_element.iowait;
    unsigned long idle = new_element.idle + new_element.iowait;

    unsigned long prev_non_idle = prev_element.user + prev_element.nice + prev_element.system 
        + prev_element.irq + prev_element.softirq + prev_element.steal;

    unsigned long non_idle = new_element.user + new_element.nice + new_element.system 
        + new_element.irq + new_element.softirq + new_element.steal;

    unsigned long prev_total = prev_idle + prev_non_idle;
    unsigned long total = idle + non_idle;

    unsigned long total_diff = total - prev_total;
    unsigned long idle_diff = idle - prev_idle;

    // printf("Total diff: %lu, Idle diff: %lu\n", total_diff, idle_diff);

    double cpu_percentage = ((double)total_diff - (double)idle_diff)*100/(double)total_diff;
    // printf("cpu: %f\n", cpu_percentage);

    return cpu_percentage;
}

void AnalyzeData() {
    CpuCoreData *cpu_data = malloc(sizeof(CpuCoreData) * context.core_no);
    bool result = DB_GetDataFromBuffer(cpu_data, BufferTypeReadData);

    // printf("Analyzer\nCPU %d - User: %lu, Nice: %lu, System: %lu, Idle: %lu, IO Wait: %lu, IRQ: %lu, Soft IRQ: %lu, Steal: %lu, Guest: %lu, Guest Nice: %lu\n",
    //            cpu_data[0].cpu_id, cpu_data[0].user, cpu_data[0].nice, cpu_data[0].system, cpu_data[0].idle, cpu_data[0].iowait,
    //            cpu_data[0].irq, cpu_data[0].softirq, cpu_data[0].steal, cpu_data[0].guest, cpu_data[0].guest_nice);

    if(result == false) {
        printf("No data in buffer\n");
        return;
    }

    double result_array[context.core_no];

    printf("------------Analyzed data-----------------\n");
    for(int i = 0; i < context.core_no; i++) {
        result_array[i] = Calculate(context.previous_cpu_each_core_data[i], cpu_data[i]);
        printf("CPU%d: %f\n",i - 1 ,result_array[i]);
    }

    DB_AddDataToBuffer(result_array, BufferTypeAnalyzedData);
    memcpy(context.previous_cpu_each_core_data, cpu_data, context.core_no * sizeof(CpuCoreData));
    free(cpu_data);
}



