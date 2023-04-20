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

    double cpu_percentage = ((double)total_diff - (double)idle_diff)*100/(double)total_diff;

    return cpu_percentage;
}

void AnalyzeData() {
    CpuCoreData *cpu_data = malloc(sizeof(CpuCoreData) * context.core_no);
    bool result = DB_GetDataFromReadDataBuffer(cpu_data);

    if(result == false) {
        printf("No data in buffer\n");
        return;
    }
    double result_array[context.core_no];

    for(int i = 0; i < context.core_no; i++) {
        result_array[i] = Calculate(context.previous_cpu_each_core_data[i], cpu_data[i]);
    }

    DB_AddDataToAnalyzedDataBuffer(result_array);
    memcpy(context.previous_cpu_each_core_data, cpu_data, context.core_no * sizeof(CpuCoreData));
    free(cpu_data);
}
