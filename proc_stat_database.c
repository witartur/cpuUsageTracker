#include <stdlib.h>
#include <stdbool.h>

#include "proc_stat_database.h"
#include "circular_buffer.h"
#include "reader.h"


static struct {
    unsigned cores_no;
} context = {0};

bool DB_Init() {
    context.cores_no = Reader_GetCoreNo();
}

unsigned DB_GetCoreNo() {
    return context.cores_no;
}

bool DB_AddDataToBuffer(void* data, BufferType buffer_type) {
    if(context.cores_no < 1)
        return false;

    CB_PushBack(data, buffer_type);
    return true;
}

bool DB_GetDataFromBuffer(void* destination, BufferType buffer_type) {
    if(context.cores_no < 1)
        return false;

    return CB_PopFront(destination, buffer_type);
}
