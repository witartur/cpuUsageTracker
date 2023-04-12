#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "proc_stat_database.h"
#include "reader.h"

int main() {
    printf("main begin\n");
    ReadProcStatFromFile();

    return 0;
}
