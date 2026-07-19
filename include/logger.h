#ifndef neon_logger_h
#define neon_logger_h

#include "file.h"

bool initLogger(const char* path);
void closeLogger(void);

void logFatal(const char* file, int line, const char* fmt, ...);

#define LOG_FATAL(...) logFatal(__FILE__, __LINE__, __VA_ARGS__)

#endif