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
#include "logger.h"

static struct {
    bool reset_needed;
    bool padding0[7];
    pthread_t thread_reader;
    pthread_t thread_analyzer;
    pthread_t thread_printer;
    pthread_t thread_watchdog;
    pthread_t thread_logger;
    pthread_mutex_t reset_flag_mutex;
} context;

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

static void TerminationHandler(int signal) {
    (void)signal;
    if(signal == -1)
        Logger_Log("MAIN: Watchdog termination");
    else
        Logger_Log("MAIN: User termination");

    pthread_cancel(context.thread_reader);
    pthread_cancel(context.thread_analyzer);
    pthread_cancel(context.thread_printer);
    pthread_cancel(context.thread_watchdog);
    pthread_cancel(context.thread_logger);
    pthread_mutex_destroy(&context.reset_flag_mutex);
}

static void* ThreadReader() {
    while(1) {
        Reader_GetProcStatFromFile();
        SetResetFlag(false);
        sleep(1);
    }
    return 0;
}

static void* ThreadAnalyzer() {
    while(1) {
        Analyzer_AnalyzeData();
        SetResetFlag(false);
    }
    return 0;
}

static void* ThreadPrinter() {
    while (1) {
        Printer_Print();
        SetResetFlag(false);
    }
    return 0;
}

static void* ThreadWatchdog() {
    while(1) {
        SetResetFlag(true);
        sleep(2);
        if(GetResetFlag() == true) {
            TerminationHandler(-1);
        }
    }
    return 0;
}

static void* ThreadLogger() {
    while(1) {
        Logger_SaveLogs();
        SetResetFlag(false);
    }
    return 0;
}

static bool CreateThreads() {
    bool result = pthread_create(&context.thread_reader, NULL, &ThreadReader, NULL) == 0
        && pthread_create(&context.thread_analyzer, NULL, &ThreadAnalyzer, NULL) == 0
        && pthread_create(&context.thread_printer, NULL, &ThreadPrinter, NULL) == 0
        && pthread_create(&context.thread_watchdog, NULL, &ThreadWatchdog, NULL) == 0
        && pthread_mutex_init(&context.reset_flag_mutex, NULL) == 0;
    
    if(result == true)
        Logger_Log("MAIN: Threads created successfully");
    return result;
}

static bool InitLogger() {
    bool result = Logger_Init()
        && pthread_create(&context.thread_logger, NULL, &ThreadLogger, NULL) == 0;
    
    if (result == true)
        Logger_Log("MAIN: Logger initialized successfully");
    
    return result;
}

static void WaitForThreadsTermination() {
    pthread_join(context.thread_reader, NULL);
    pthread_join(context.thread_analyzer, NULL);
    pthread_join(context.thread_printer, NULL);
    pthread_join(context.thread_watchdog, NULL);
    pthread_join(context.thread_logger, NULL);
}

static bool Init() {
    bool result = Reader_Init()
        && DB_Init()
        && Analyzer_Init();
    
    if(result == true)
        Logger_Log("MAIN: All components initialized successfully");
    
    return result;
}

static void Deinit() {
    DB_DeInit();
    Logger_DeInit();
    Analyzer_DeInit();
}

int main() {
    if (InitLogger() == false)
        return 0;

    signal(SIGINT, TerminationHandler);
    signal(SIGTERM, TerminationHandler);

    if(Init() == false)
       return 0;

    if(CreateThreads() == false)
        return 0;

    WaitForThreadsTermination();
    Deinit();
    return 1;
}
