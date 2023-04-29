#ifndef LOGGER_H
#define LOGGER_H

#define MAX_MSG_SIZE 256

bool Logger_Init();
void Logger_Log(char *log_message);
void Logger_SaveLogs();
void Logger_DeInit();

#endif  // LOGGER_H
