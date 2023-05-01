#ifndef ANALYZER_H
#define ANALYZER_H

#include "proc_stat_database.h"

bool Analyzer_Init(void);
void Analyzer_AnalyzeData(void);
void Analyzer_DeInit(void);

// for utests
double Calculate(CpuCoreData prev_element, CpuCoreData new_element);

#endif  // ANALYZER_H
