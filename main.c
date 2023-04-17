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

    int counter;

    sleep(2);
    for(counter = 1; counter <=5; counter++) {
    printf("*********ReadProcStatFromFile %d begin*******\n", counter);
    ReadProcStatFromFile();

    printf("*********AnalyzeData %d begin*******\n", counter);
    AnalyzeData();

    printf("*********Printer %d begin*******\n", counter);
    Printer();

    sleep(2);
    }

    printf("*********CB*******\n");
    CB_Free();

    return 1;
}
