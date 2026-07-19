#include "logger.h"

#include <stdarg.h>
#include <string.h>
#include <time.h>

typedef enum {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

static const char* levelNames[] = {
    [LOG_TRACE] = "TRACE", [LOG_DEBUG] = "DEBUG", [LOG_INFO] = "INFO",
    [LOG_WARN] = "WARN",   [LOG_ERROR] = "ERROR", [LOG_FATAL] = "FATAL"};

File logFile;

bool initLogger(const char* path) {
    return fileOpen(&logFile, path, FILE_APPEND);
}

void closeLogger(void) { fileClose(&logFile); }

static void vlog(LogLevel level, const char* file, int line, const char* fmt,
                 va_list args) {
    char buffer[2048];
    size_t offset = 0;

    time_t now = time(NULL);
    struct tm tm;

#ifdef _WIN32
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif

    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                       "[%04d-%02d-%02d %02d:%02d:%02d] %-5s %s:%d: ",
                       tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
                       tm.tm_min, tm.tm_sec, levelNames[level], file, line);

    offset += vsnprintf(buffer + offset, sizeof(buffer) - offset, fmt, args);

    if (offset >= sizeof(buffer) - 2) {
        offset = sizeof(buffer) - 2;
    }

    buffer[offset++] = '\n';

    fileWrite(&logFile, buffer, offset);
}

void logFatal(const char* file, int line, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vlog(LOG_FATAL, file, line, fmt, args);
    va_end(args);
}