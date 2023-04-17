#include <stdio.h>
#include <stdlib.h>

#include "printer.h"
#include "proc_stat_database.h"
#include "circular_buffer.h"


void Printer() {
    unsigned core_no = DB_GetCoreNo();
    printf("Printer core_no: %d\n", core_no);
    double *analized_cpu_data = malloc(sizeof(double) * core_no);
    bool result = DB_GetDataFromBuffer(analized_cpu_data, BufferTypeAnalyzedData);

    if(result == false) {
      printf("No data in buffer\n");
      return;
    }

    // system("clear");
    printf("\n\n");
    printf("-------   CPU USAGE TRACKER   -------\n\n");

    int i;
    printf("   CPU: %.2f%%\n\n", analized_cpu_data[0]);
    printf("CORE:\n");
    for (i = 1; i < core_no; i++) {
      printf("CPU%d: %.2f%%\n", i-1, analized_cpu_data[i]);
    }

    free(analized_cpu_data);
}
