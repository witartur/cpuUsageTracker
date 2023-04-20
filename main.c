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

static bool Deinit() {
    return CB_Free && DB_Denit;
}

void* ThreadReader() {
    while(1) {
        Reader_GetProcStatFromFile();
        sleep(1);
    }
}

void* ThreadAnalyzer() {
    while(1) {
        AnalyzeData();
    }
}

void* ThreadPrinter() {
    while (1) {
        Printer();
    }
}

int main() {
    pthread_t t1, t2, t3;

    if(Init() == false)
        return 0;

    if (pthread_create(&t1, NULL, &ThreadReader, NULL) != 0)
        printf("Handle Error");

    if (pthread_create(&t2, NULL, &ThreadAnalyzer, NULL) != 0)
        printf("Handle Error");

    if (pthread_create(&t3, NULL, &ThreadPrinter, NULL) != 0)
        printf("Handle Error");

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
        pthread_join(t3, NULL);

    if(Deinit() == false)
        return 0;

    return 1;
}
