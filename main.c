#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <limits.h>
#include <float.h>
#include <signal.h>
#include <string.h>

#include "proc_stat_database.h"
#include "reader.h"
#include "circular_buffer.h"
#include "analyzer.h"
#include "printer.h"

static struct {
    pthread_t thread_reader;
    pthread_t thread_analyzer;
    pthread_t thread_printer;
} context;

static bool Init() {
    return Reader_Init()
        && DB_Init()
        && CB_Init()
        && Analyzer_Init();
}

static bool Deinit() {
    return CB_Free && DB_DeInit;
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

void TerminationHandler(int signal) {
    (void)signal;
    pthread_cancel(context.thread_reader);
    pthread_cancel(context.thread_analyzer);
    pthread_cancel(context.thread_printer);
    printf("Tread cancel completed\n");
}

int main() {
    signal(SIGINT, TerminationHandler);
    signal(SIGTERM, TerminationHandler);

    if(Init() == false)
        return 0;

    if (pthread_create(&context.thread_reader, NULL, &ThreadReader, NULL) != 0)
        printf("Handle Error");

    if (pthread_create(&context.thread_analyzer, NULL, &ThreadAnalyzer, NULL) != 0)
        printf("Handle Error");

    if (pthread_create(&context.thread_printer, NULL, &ThreadPrinter, NULL) != 0)
        printf("Handle Error");

        pthread_join(context.thread_reader, NULL);
        pthread_join(context.thread_analyzer, NULL);
        pthread_join(context.thread_printer, NULL);

    if(Deinit() == false)
        return 0;

    printf("Deinitilization completed\n");

    return 1;
}
