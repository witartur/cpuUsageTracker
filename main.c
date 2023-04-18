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
#include "printer.h"

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

    for(int counter = 1; counter <=5; counter++) {
        Reader_GetProcStatFromFile();
        AnalyzeData();
        Printer();

        sleep(2);
    }
    CB_Free();

    return 1;
}
