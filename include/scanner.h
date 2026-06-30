#ifndef neon_scanner_h
#define neon_scanner_h

#include "common.h"

typedef enum {
    TOKEN_COMMANDS_START,

    // Connection
    TOKEN_PING,
    TOKEN_ECHO,
    TOKEN_QUIT,

    // CRUD Op
    TOKEN_SET,
    TOKEN_GET,
    TOKEN_DEL,
    TOKEN_KEYS,

    // Key management
    TOKEN_RENAME,
    TOKEN_EXISTS,
    TOKEN_TYPE,

    // DB management
    TOKEN_FLUSH,
    TOKEN_DBSIZE,

    TOKEN_COMMANDS_END,

    // Constants
    TOKEN_NUMBER,
    TOKEN_STRING,

    // Special
    TOKEN_EOF,
    TOKEN_ERROR

} TokenType;

typedef struct {
    TokenType type;
    const char* start;
    int length;
    int line;
} Token;

void initScanner(const char* source);
Token scanToken();

#endif