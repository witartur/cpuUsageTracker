#ifndef LOGGER_H
#define LOGGER_H

#define MAX_MSG_SIZE 256

bool Logger_Init(void);
void Logger_Log(char *log_message);
void Logger_SaveLogs(void);
void Logger_DeInit(void);

#endif  // LOGGER_H
