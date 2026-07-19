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

void logFatal(const char* fmt, ...) {}