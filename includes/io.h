#ifndef IO_H
#define IO_H

#include <sv.h>

#define LOG_FILENAME ".zi_log"

typedef enum { 
    LOG_INFO, 
    LOG_WARNING, 
    LOG_FATAL, 

    // Must be last
    _LOG_COUNT
} LOG_PRIORITY;


void log_init();
void log_close();
void log_print(LOG_PRIORITY priority, char* fmt, ...);

#endif // IO_H
