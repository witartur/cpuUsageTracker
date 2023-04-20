#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#include "proc_stat_database.h"
#include "circular_buffer.h"
#include "reader.h"


static struct {
    unsigned cores_no;
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

    context.cores_no = Reader_GetCoreNo();
    return result;
}

bool DB_Denit() {
    bool result = pthread_mutex_destroy(&context.mutex_buff_read_data) == 0
        && pthread_mutex_destroy(&context.mutex_buff_analyzed_data) == 0
        && sem_destroy(&context.sem_buff_read_data_empty) == 0
        && sem_destroy(&context.sem_buff_read_data_full) == 0
        && sem_destroy(&context.sem_buff_analyzed_data_empty) == 0
        && sem_destroy(&context.sem_buff_analyzed_data_full) == 0;
}

unsigned DB_GetCoreNo() {
    return context.cores_no;
}

bool DB_AddDataToReadDataBuffer(CpuCoreData *data) {
    sem_wait(&context.sem_buff_read_data_empty);
    pthread_mutex_lock(&context.mutex_buff_read_data);

    bool result = CB_PushBack(data, BufferTypeReadData);

    pthread_mutex_unlock(&context.mutex_buff_read_data);
    sem_post(&context.sem_buff_read_data_full);

    return result;
}

bool DB_GetDataFromReadDataBuffer(void* destination) {
    sem_wait(&context.sem_buff_read_data_full);
    pthread_mutex_lock(&context.mutex_buff_read_data);

    bool result = CB_PopFront(destination, BufferTypeReadData);

    pthread_mutex_unlock(&context.mutex_buff_read_data);
    sem_post(&context.sem_buff_read_data_empty);

    return result;
}

bool DB_AddDataToAnalyzedDataBuffer(double *data) {
    sem_wait(&context.sem_buff_analyzed_data_empty);
    pthread_mutex_lock(&context.mutex_buff_analyzed_data);

    bool result = CB_PushBack(data, BufferTypeAnalyzedData);

    pthread_mutex_unlock(&context.mutex_buff_analyzed_data);
    sem_post(&context.sem_buff_analyzed_data_full);

    return true;
}

bool DB_GetDataFromAnalyzedDataBuffer(void* destination) {
    sem_wait(&context.sem_buff_analyzed_data_full);
    pthread_mutex_lock(&context.mutex_buff_analyzed_data);

    bool result = CB_PopFront(destination, BufferTypeAnalyzedData);

    pthread_mutex_unlock(&context.mutex_buff_analyzed_data);
    sem_post(&context.sem_buff_analyzed_data_empty);

    return result;
}
