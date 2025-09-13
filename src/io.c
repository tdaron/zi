#include "io.h"
#include <editor.h>
#include <stdarg.h>
#include <time.h>

void log_init()
{
    editor.logFileHandle = fopen(LOG_FILENAME, "ab+");
    int c = fgetc(editor.logFileHandle);
    if (c == EOF) {
        // If the log file is empty, add UTF8-BOM at the beginning
        // as logs might contain some utf8
        fputc(0xEF, editor.logFileHandle);
        fputc(0xBB, editor.logFileHandle);
        fputc(0xBF, editor.logFileHandle);
    } else {
        ungetc(c, editor.logFileHandle);
    }
    log_print(LOG_INFO, "aaah");
}

void log_close()
{
    if (editor.logFileHandle)
        fclose(editor.logFileHandle);
}

void log_print(LOG_PRIORITY priority, char* fmt, ...)
{
    va_list parametersInfos;
    va_start(parametersInfos, fmt);

    if (!editor.logFileHandle)
        return;

    if (priority > _LOG_COUNT)
        return;
    char* priorityMessage[_LOG_COUNT];
    priorityMessage[LOG_INFO] = "INFO";
    priorityMessage[LOG_WARNING] = "WARN";
    priorityMessage[LOG_FATAL] = "FATAL";

    time_t raw_time;
    struct tm* timeinfo;

    time(&raw_time);
    timeinfo = localtime(&raw_time);

    fprintf(editor.logFileHandle, "[%s] %d-%02d-%02d %02d:%02d:%02d ", priorityMessage[priority],
        timeinfo->tm_year + 1900, timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min,
        timeinfo->tm_sec);
    vfprintf(editor.logFileHandle, fmt, parametersInfos);
    fprintf(editor.logFileHandle, "\n");
    fflush(editor.logFileHandle);
}
