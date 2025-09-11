#ifndef IO_H
#define IO_H

#include <sv.h>
#include <time.h>
#include <stdio.h>

#define LOG_FILENAME ".zi_log"

typedef enum { 
    LOG_INFO, 
    LOG_WARNING, 
    LOG_FATAL, 

    // Must be last
    _LOG_COUNT
} LOG_PRIORITY;

typedef struct {
    time_t startTime;
    FILE *file;
} Log;

Log log_init();
void log_close(Log logFileHandle);
void log_print(Log logFileHandle, const char *message, LOG_PRIORITY priority);

#endif // IO_H