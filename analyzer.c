#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "analyzer.h"
#include "logger.h"

static struct {
    unsigned core_no;
    int padding0;
    CpuCoreData *previous_cpu_each_core_data;
} context = {0};

bool Analyzer_Init() {
    context.core_no = DB_GetCoreNo();
    context.previous_cpu_each_core_data = malloc(sizeof(CpuCoreData) * context.core_no);

    if (context.previous_cpu_each_core_data == NULL) {
        Logger_Log("ANALYZER: Init ERROR - Can't allocate memory");
        return false;
    }

    memset(context.previous_cpu_each_core_data, 0, sizeof(CpuCoreData) * context.core_no);
    return true;
}

void Analyzer_DeInit() {
    free(context.previous_cpu_each_core_data);
}

double Calculate(CpuCoreData prev_element, CpuCoreData new_element) {
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

void Analyzer_AnalyzeData() {
    CpuCoreData cpu_data[context.core_no];
    DB_GetDataFromReadDataBuffer(cpu_data);

    double result_array[context.core_no];

    for(unsigned i = 0; i < context.core_no; i++) {
        result_array[i] = Calculate(context.previous_cpu_each_core_data[i], cpu_data[i]);
    }

    DB_AddDataToAnalyzedDataBuffer(result_array);
    Logger_Log("ANALYZER: Data analyzed and stored successfully");

    memcpy(context.previous_cpu_each_core_data, cpu_data, context.core_no * sizeof(CpuCoreData));
}
