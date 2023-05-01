#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#include "logger.h"
#include "proc_stat_database.h"
#include "circular_buffer.h"

#define FILELOCATION "logger.csv"

static struct {
    pthread_mutex_t mutex_buff_logger;
    sem_t sem_buff_logger_empty;
    sem_t sem_buff_logger_full;
} context = {0};

static void GetTimeStamp(char *log_timestamp) {
    time_t rawtime;
    time(&rawtime);
    
    strncpy(log_timestamp, ctime(&rawtime), 32);
}

static void Write(const char *log) {
    FILE* log_file;
    log_file = fopen(FILELOCATION, "a");

    if (log_file == NULL) {
        printf("Failed to write to csv file");
        return; 
    }
    fprintf(log_file, "%s", log);

    fclose(log_file);
    log_file = NULL;
}

static void GetDataFromLoggerBuffer(void* destination) {
    sem_wait(&context.sem_buff_logger_full);
    pthread_mutex_lock(&context.mutex_buff_logger);

    CB_PopFront(destination, BufferTypeLogger);

    pthread_mutex_unlock(&context.mutex_buff_logger);
    sem_post(&context.sem_buff_logger_empty);
}

static void AddDataToLoggerBuffer(char *data) {
    sem_wait(&context.sem_buff_logger_empty);
    pthread_mutex_lock(&context.mutex_buff_logger);

    CB_PushBack(data, BufferTypeLogger);

    pthread_mutex_unlock(&context.mutex_buff_logger);
    sem_post(&context.sem_buff_logger_full);
}

bool Logger_Init() {
    bool result = CB_Init(BufferTypeLogger)
        && pthread_mutex_init(&context.mutex_buff_logger, NULL) == 0
        && sem_init(&context.sem_buff_logger_empty, 0, BUFFER_SIZE) == 0
        && sem_init(&context.sem_buff_logger_full, 0, 0) == 0;

    printf("CB init: %d", result);

    FILE* log_file;
    log_file = fopen(FILELOCATION, "w");

    if (log_file == NULL) {
        printf("Failed to write to csv file");
        return false; 
    }
    fprintf(log_file, "LOG MSG,TIMESTAMP\n");

    fclose(log_file);
    log_file = NULL;
    return true;
}

void Logger_DeInit() {
    pthread_mutex_destroy(&context.mutex_buff_logger);
    sem_destroy(&context.sem_buff_logger_empty);
    sem_destroy(&context.sem_buff_logger_full);
}

void Logger_Log(char *log_message) {
    char log_timestamp[32] = {0};
    GetTimeStamp(log_timestamp);

    char log[MAX_MSG_SIZE] = {0};
    strncpy(log, log_message, strlen(log_message));
    strncat(log, ",", sizeof(","));
    strncat(log, log_timestamp, strlen(log_timestamp)); 
    
    AddDataToLoggerBuffer(log);
}

void Logger_SaveLogs() {
    char log[MAX_MSG_SIZE] = {0};

    GetDataFromLoggerBuffer(log);
    Write(log);
}
