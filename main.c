#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "proc_stat_database.h"
#include "reader.h"
#include "circular_buffer.h"

static bool Init() {
    return ReaderInit()
        && DB_Init()
        && CB_Init();
}

int main() {
    printf("main begin\n");

    if(Init() == false)
        return 0;

    ReadProcStatFromFile();
    printf("END OF PROCSTAT");
    ReadProcStatFromFile();

    CB_Print();
    CB_Free();

    return 1;
}
