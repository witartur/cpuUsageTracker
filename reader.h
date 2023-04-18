#ifndef READER_H
#define READER_H

#include <stdbool.h>

bool Reader_Init();
unsigned Reader_GetCoreNo();
bool Reader_GetProcStatFromFile();

#endif //READER_H
