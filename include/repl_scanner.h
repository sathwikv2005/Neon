#ifndef neon_scanner_h
#define neon_scanner_h

#include "common.h"

typedef enum NeonTokenType {
    TOKEN_COMMANDS_START,

    // Connection
    TOKEN_PING,
    TOKEN_ECHO,
    TOKEN_QUIT,
    TOKEN_EXIT,

    // number operations
    TOKEN_INCR,
    TOKEN_DECR,

    // CRUD Op
    TOKEN_SET,
    TOKEN_GET,
    TOKEN_DEL,
    TOKEN_KEYS,

    // Key management
    TOKEN_RENAME,
    TOKEN_EXISTS,
    TOKEN_TYPEOF,

    // DB management
    TOKEN_SELECT,
    TOKEN_FLUSH,
    TOKEN_DBSIZE,
    TOKEN_SAVE,

    // Server
    TOKEN_INFO,

    TOKEN_COMMANDS_END,

    // Constants
    TOKEN_NUMBER,
    TOKEN_STRING,

    // Special
    TOKEN_EOF,
    TOKEN_ERROR

} NeonTokenType;

typedef struct {
    NeonTokenType type;
    const char* start;
    int length;
    int line;
} Token;

void initScanner(const char* source);
Token scanToken();

#endif