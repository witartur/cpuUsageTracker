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
#include "analyzer.h"
#include "printer.h"

static struct {
    pthread_t thread_reader;
    pthread_t thread_analyzer;
    pthread_t thread_printer;
    pthread_t thread_watchdog;

    bool reset_needed;
    pthread_mutex_t reset_flag_mutex;
} context;

static bool Deinit() {
    return DB_DeInit();
}

static void SetResetFlag(bool state) {
    pthread_mutex_lock(&context.reset_flag_mutex);
    context.reset_needed = state;
    pthread_mutex_unlock(&context.reset_flag_mutex);
}

static bool GetResetFlag() {
    bool state = false;
    pthread_mutex_lock(&context.reset_flag_mutex);
    state = context.reset_needed;
    pthread_mutex_unlock(&context.reset_flag_mutex);
    return state;
}

static void* ThreadReader() {
    // int i = 0;
    while(1) {
        // i++;
        Reader_GetProcStatFromFile();
        SetResetFlag(false);
        sleep(1);
        // if(i == 3)
        //     sleep(5);
    }
}

static void* ThreadAnalyzer() {
    while(1) {
        AnalyzeData();
        SetResetFlag(false);
    }
}

static void* ThreadPrinter() {
    while (1) {
        Printer();
        SetResetFlag(false);
    }
}

void TerminationHandler(int signal) {
    (void)signal;
    pthread_cancel(context.thread_reader);
    pthread_cancel(context.thread_analyzer);
    pthread_cancel(context.thread_printer);
    pthread_cancel(context.thread_watchdog);
    pthread_mutex_destroy(&context.reset_flag_mutex);
    printf("Tread cancel completed\n");
}

static void* ThreadWatchdog() {
    while(1) {
        SetResetFlag(true);
        sleep(2);
        if(GetResetFlag() == true) {
            TerminationHandler(0);
        }
    }
}

static bool CreateThreads() {
    return pthread_create(&context.thread_reader, NULL, &ThreadReader, NULL) == 0
        && pthread_create(&context.thread_analyzer, NULL, &ThreadAnalyzer, NULL) == 0
        && pthread_create(&context.thread_printer, NULL, &ThreadPrinter, NULL) == 0
        && pthread_create(&context.thread_watchdog, NULL, &ThreadWatchdog, NULL) == 0
        && pthread_mutex_init(&context.reset_flag_mutex, NULL) == 0;
}

static bool Init() {
    return Reader_Init()
        && DB_Init()
        && Analyzer_Init()
        && CreateThreads();
}

int main() {
    signal(SIGINT, TerminationHandler);
    signal(SIGTERM, TerminationHandler);

    if(Init() == false)
        return 0;

    pthread_join(context.thread_reader, NULL);
    pthread_join(context.thread_analyzer, NULL);
    pthread_join(context.thread_printer, NULL);
    pthread_join(context.thread_watchdog, NULL);

    if(Deinit() == false)
        return 0;

    printf("Deinitilization completed\n");

    return 1;
}
