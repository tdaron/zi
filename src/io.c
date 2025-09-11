#include "io.h"

Log log_init() {
    Log log = { 0 };
    
    log.startTime = time(NULL);
    log.file = fopen(LOG_FILENAME, "w");

    return log;
}

void log_close(Log log) {
    if (log.file) fclose(log.file);
}

void log_print(Log log, String_View message, LOG_PRIORITY priority) {
    if (!log.file) return;

    if (priority > _LOG_COUNT) return;
    char *priorityMessage[_LOG_COUNT];
    priorityMessage[LOG_INFO] = "Info";
    priorityMessage[LOG_WARNING] = "Warning";
    priorityMessage[LOG_FATAL] = "Fatal";

    snprintf(log.buffer, MAX_LOG_SIZE, "[%s - %lds] %s", priorityMessage[priority], time(NULL)-log.startTime, message.data);
    fwrite(log.buffer, 1, strlen(log.buffer), log.file);
    // Logs appear on the spot
    fflush(log.file);
}