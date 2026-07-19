#ifndef neon_logger_h
#define neon_logger_h

#include "file.h"

bool initLogger(const char* path);
void closeLogger(void);

void logTrace(const char* file, int line, const char* fmt, ...);
void logDebug(const char* file, int line, const char* fmt, ...);
void logInfo(const char* file, int line, const char* fmt, ...);
void logWarn(const char* file, int line, const char* fmt, ...);
void logError(const char* file, int line, const char* fmt, ...);
void logFatal(const char* file, int line, const char* fmt, ...);

#define LOG_TRACE(...) logTrace(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) logDebug(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...) logInfo(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...) logWarn(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) logError(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) logFatal(__FILE__, __LINE__, __VA_ARGS__)

#endif