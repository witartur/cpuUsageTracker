#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "proc_stat_database.h"
#include "reader.h"
#include "circular_buffer.h"

int main() {
    printf("main begin\n");

    ReaderInit();
    CB_Init();

    ReadProcStatFromFile();
    printf("END OF PROCSTAT");
    ReadProcStatFromFile();

    CB_Print();
    CB_Free();

    return 0;
}
