#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#include "proc_stat_database.h"
#include "circular_buffer.h"
#include "reader.h"
#include "logger.h"

static struct {
    unsigned cores_no;
    int padding0;
    pthread_mutex_t mutex_buff_read_data;
    pthread_mutex_t mutex_buff_analyzed_data;
    sem_t sem_buff_read_data_empty;
    sem_t sem_buff_read_data_full;
    sem_t sem_buff_analyzed_data_empty;
    sem_t sem_buff_analyzed_data_full;
} context = {0};

bool DB_Init() {
    bool result = pthread_mutex_init(&context.mutex_buff_read_data, NULL) == 0
        && pthread_mutex_init(&context.mutex_buff_analyzed_data, NULL) == 0
        && sem_init(&context.sem_buff_read_data_empty, 0, BUFFER_SIZE) == 0
        && sem_init(&context.sem_buff_read_data_full, 0, 0) == 0
        && sem_init(&context.sem_buff_analyzed_data_empty, 0, BUFFER_SIZE) == 0
        && sem_init(&context.sem_buff_analyzed_data_full, 0, 0) == 0;

    if(result == false)
        Logger_Log("DB: Init Error");

    context.cores_no = Reader_GetCoreNo();
    
    result = CB_Init(BufferTypeReadData) && CB_Init(BufferTypeAnalyzedData);

    if(result == false)
        Logger_Log("DB: Init Error, Can't allocate buffer memory");

    return result;
}

void DB_DeInit() {
    pthread_mutex_destroy(&context.mutex_buff_read_data);
    pthread_mutex_destroy(&context.mutex_buff_analyzed_data);
    sem_destroy(&context.sem_buff_read_data_empty);
    sem_destroy(&context.sem_buff_read_data_full);
    sem_destroy(&context.sem_buff_analyzed_data_empty);
    sem_destroy(&context.sem_buff_analyzed_data_full);

    CB_Free();
}

unsigned DB_GetCoreNo() {
    return context.cores_no;
}

void DB_AddDataToReadDataBuffer(CpuCoreData *data) {
    sem_wait(&context.sem_buff_read_data_empty);
    pthread_mutex_lock(&context.mutex_buff_read_data);

    CB_PushBack(data, BufferTypeReadData);

    pthread_mutex_unlock(&context.mutex_buff_read_data);
    sem_post(&context.sem_buff_read_data_full);
}

void DB_GetDataFromReadDataBuffer(void* destination) {
    sem_wait(&context.sem_buff_read_data_full);
    pthread_mutex_lock(&context.mutex_buff_read_data);

    CB_PopFront(destination, BufferTypeReadData);

    pthread_mutex_unlock(&context.mutex_buff_read_data);
    sem_post(&context.sem_buff_read_data_empty);
}

void DB_AddDataToAnalyzedDataBuffer(double *data) {
    sem_wait(&context.sem_buff_analyzed_data_empty);
    pthread_mutex_lock(&context.mutex_buff_analyzed_data);

    CB_PushBack(data, BufferTypeAnalyzedData);

    pthread_mutex_unlock(&context.mutex_buff_analyzed_data);
    sem_post(&context.sem_buff_analyzed_data_full);
}

void DB_GetDataFromAnalyzedDataBuffer(void* destination) {
    sem_wait(&context.sem_buff_analyzed_data_full);
    pthread_mutex_lock(&context.mutex_buff_analyzed_data);

    CB_PopFront(destination, BufferTypeAnalyzedData);

    pthread_mutex_unlock(&context.mutex_buff_analyzed_data);
    sem_post(&context.sem_buff_analyzed_data_empty);
}
