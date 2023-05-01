#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "analyzer.h"
#include "circular_buffer.h"
#include "proc_stat_database.h"

static void GetRandomCpuCoreDataTable(CpuCoreData* destination) {
    unsigned cores_no = DB_GetCoreNo();

    for(int i = 0; i < (int) cores_no; i++) {
        CpuCoreData element = {
            .cpu_id = i,
            .user = 96464,
            .nice = 181,
            .system = 45592,
            .idle = 2627649,
            .iowait = 34131,
            .irq = 0,
            .softirq = 590,
            .steal = 0,
            .guest = 0,
            .guest_nice = 0
        };

        destination[i] = element;
    }
}

static void test_ReadElementPushedAndPopCorectly() {
    size_t elements_no = CB_GetNoOfElements(BufferTypeReadData);
    assert(elements_no == 0);

    size_t table_size = (size_t) (DB_GetCoreNo() * sizeof(CpuCoreData));

    CpuCoreData *read_data_table = malloc(table_size);
    GetRandomCpuCoreDataTable(read_data_table);

    CB_PushBack(read_data_table, BufferTypeReadData);
    elements_no = CB_GetNoOfElements(BufferTypeReadData);
    assert(elements_no == 1);

    CpuCoreData *pop_data_table = malloc(table_size);
    CB_PopFront(&pop_data_table, BufferTypeReadData);
    elements_no = CB_GetNoOfElements(BufferTypeReadData);
    assert(elements_no == 0);
    assert((memcmp(read_data_table, pop_data_table, table_size) == 0));

    free(read_data_table);
    free(pop_data_table);

    printf("test_ReadElementPushedAndPopCorectly PASSED\n");
}

static void SetCoreNoInDBContext() {
    DB_Init();
    DB_DeInit();
}

static void test_CircularBuffer() {
    SetCoreNoInDBContext();
    CB_Init(BufferTypeReadData);
    CB_Init(BufferTypeAnalyzedData);
    CB_Init(BufferTypeLogger);

    test_ReadElementPushedAndPopCorectly();

    CB_Free();
}

static void test_CalculationsShouldBeCorrect() {
    CpuCoreData previous_element = {
        .user = 96464,
        .nice = 181,
        .system = 45592,
        .idle = 2627649,
        .iowait = 34131,
        .irq = 0,
        .softirq = 590,
        .steal = 0,
        .guest = 0,
        .guest_nice = 0
    };

    CpuCoreData current_element = {
        .user = 96474,
        .nice = 181,
        .system = 45593,
        .idle = 2627740,
        .iowait = 34131,
        .irq = 0,
        .softirq = 590,
        .steal = 0,
        .guest = 0,
        .guest_nice = 0
    };

    double result = Calculate(previous_element, current_element);   // 10.784314

    assert(result >= 10.7843 && result <= 10.7844);
    printf("test_AnalyzerCalculationsShouldBeCorrect PASSED\n");
}

static void test_CalculationsShouldBeIncorrect() {
    CpuCoreData previous_element = {
        .user = 96464,
        .nice = 181,
        .system = 45592,
        .idle = 2627645,
        .iowait = 34131,
        .irq = 0,
        .softirq = 590,
        .steal = 0,
        .guest = 0,
        .guest_nice = 0
    };

    CpuCoreData current_element = {
        .user = 96474,
        .nice = 181,
        .system = 45593,
        .idle = 2627740,
        .iowait = 34131,
        .irq = 0,
        .softirq = 590,
        .steal = 0,
        .guest = 0,
        .guest_nice = 0
    };

    double result = Calculate(previous_element, current_element);   // 10.784314

    assert((result >= 10.7843 && result <= 10.7844) == false);
    printf("test_AnalyzerCalculationsShouldBeIncorrect PASSED\n");
}

static void test_Analyzer() {
    test_CalculationsShouldBeCorrect();
    test_CalculationsShouldBeIncorrect();
}


int main() {
    test_CircularBuffer();
    test_Analyzer();

    return 1;
}
