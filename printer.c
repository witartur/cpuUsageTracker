#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "printer.h"
#include "proc_stat_database.h"
#include "logger.h"

static void PrintToTerminal(double *analized_cpu_data, unsigned core_no) {
  system("clear");
  printf("Termination command: kill -TERM %d\n\n", getpid());
  printf("-------   CPU USAGE TRACKER   -------\n\n");
  
  printf("\tCPU: %.2f%%\n\n", analized_cpu_data[0]);
  for (unsigned i = 1; i < core_no; i++) {
    printf("CPU%d: %.2f%%\n", i-1, analized_cpu_data[i]);
  }
}

void Printer_Print() {
  unsigned core_no = DB_GetCoreNo();
  double analized_cpu_data[core_no];

  DB_GetDataFromAnalyzedDataBuffer(analized_cpu_data);
  PrintToTerminal(analized_cpu_data, core_no);

  Logger_Log("PRINTER: Data printed");
}
