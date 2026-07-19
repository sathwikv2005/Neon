#ifndef neon_common_h
#define neon_common_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NAN_BOXING
#define UINT8_COUNT (UINT8_MAX + 1)

#define NEON_VERSION_STRING "0.1.0"

#define LOG_FILE_PATH "./logs/log.txt"

#define ANSI_RED "\x1b[31m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_CYAN "\033[36m"
#define ANSI_NEON_PINK "\x1b[38;2;255;16;128m"
#define ANSI_NEON_PURPLE "\x1b[38;2;200;64;255m"
#define ANSI_NEON_BLUE "\x1b[38;2;0;220;255m"
#define ANSI_NEON_CYAN "\x1b[38;2;0;255;255m"
#define ANSI_NEON_GREEN "\x1b[38;2;57;255;20m"
#define ANSI_BOLD "\x1b[1m"
#define ANSI_ITALIC "\033[3m"
#define ANSI_DIM "\x1b[2m"
#define ANSI_RESET "\x1b[0m"

#endif