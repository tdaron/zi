#ifndef IO_H
#define IO_H

#include <sv.h>
#include <time.h>
#include <stdio.h>

#define LOG_FILENAME ".zi_log"
#define MAX_LOG_SIZE 64

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
    char buffer[MAX_LOG_SIZE];
} Log;

Log log_init();
void log_close(Log log);
void log_print(Log log, String_View message, LOG_PRIORITY priority);

#endif // IO_H