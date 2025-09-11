#include "io.h"

Log log_init() {
    Log logFileHandle = { 0 };
    
    logFileHandle.startTime = time(NULL);
    logFileHandle.file = fopen(LOG_FILENAME, "w");

    return logFileHandle;
}

void log_close(Log logFileHandle) {
    if (logFileHandle.file) fclose(logFileHandle.file);
}

void log(Log logFileHandle, String_View message, LOG_PRIORITY priority) {
    if (!logFileHandle.file) return;

    if (priority > _LOG_COUNT) return;
    char *priorityMessage[_LOG_COUNT];
    priorityMessage[LOG_INFO] = "Info";
    priorityMessage[LOG_WARNING] = "Warning";
    priorityMessage[LOG_FATAL] = "Fatal";

    snprintf(logFileHandle.buffer, MAX_LOG_SIZE, "[%s - %lds] %s", 
        priorityMessage[priority], 
        time(NULL)-logFileHandle.startTime, 
        message.data
    );
    fwrite(logFileHandle.buffer, 1, strlen(logFileHandle.buffer), logFileHandle.file);
    // Logs appear on the spot
    fflush(logFileHandle.file);
}