#include "io.h"
#include <editor.h>

Log log_init() {
    Log logFileHandle = { 0 };
    
    logFileHandle.startTime = time(NULL);
    logFileHandle.file = fopen(LOG_FILENAME, "w");

    return logFileHandle;
}

void log_close(Log logFileHandle) {
    if (logFileHandle.file) fclose(logFileHandle.file);
}

void log_print(const char *message, LOG_PRIORITY priority) {
    Log logFileHandle = editor.logFileHandle;
    if (!logFileHandle.file) return;

    if (priority > _LOG_COUNT) return;
    char *priorityMessage[_LOG_COUNT];
    priorityMessage[LOG_INFO] = "Info";
    priorityMessage[LOG_WARNING] = "Warning";
    priorityMessage[LOG_FATAL] = "Fatal";

    zi_log("[%s - %lds] %s\n",
        priorityMessage[priority], 
        time(NULL)-logFileHandle.startTime, 
        message
    );
}
