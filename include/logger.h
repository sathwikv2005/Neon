#ifndef neon_logger_h
#define neon_logger_h

#include "file.h"

void logFatal(const char* fmt, ...);
bool initLogger(const char* path);
void closeLogger(void);

#endif