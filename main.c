#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <limits.h>
#include <float.h>

#include "proc_stat_database.h"
#include "reader.h"
#include "circular_buffer.h"
#include "analyzer.h"

static bool Init() {
    return Reader_Init()
        && DB_Init()
        && CB_Init()
        && Analyzer_Init();
}

int main() {
    printf("main begin\n");

    if(Init() == false)
        return 0;

    ReadProcStatFromFile();
    AnalyzeData();

    sleep(1);

    ReadProcStatFromFile();
    AnalyzeData();

    // CB_Print();
    CB_Free();

    return 1;
}
