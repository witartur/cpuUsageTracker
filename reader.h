#ifndef READER_H
#define READER_H

#include <stdbool.h>

bool Reader_Init(void);
unsigned Reader_GetCoreNo(void);
bool Reader_GetProcStatFromFile(void);

#endif //READER_H
