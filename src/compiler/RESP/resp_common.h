#ifndef NEON_RESP_COMMON_H
#define NEON_RESP_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../compiler_common.h"

typedef enum {
    RESP_SIMPLE_STRING,
    RESP_ERROR,
    RESP_INTEGER,
    RESP_BULK_STRING,
    RESP_ARRAY,
    RESP_NULL
} RespType;

typedef struct RespValue RespValue;

struct RespValue {
    RespType type;

    union {
        int64_t integer;

        struct {
            char* data;
            size_t length;
        } string;

        struct {
            RespValue* values;
            size_t count;
        } array;
    };
};

typedef enum {
    RESP_PARSE_OK,
    RESP_PARSE_INCOMPLETE,
    RESP_PARSE_ERROR
} RespParseResult;

// typedef struct Compiler Compiler;

bool compileCommand(Compiler* compiler, const RespValue* request);

RespParseResult parseResp(const char* data, size_t length, RespValue* value);

void freeRespValue(RespValue* value);

// commands
bool compileCommand(Compiler* compiler, const RespValue* request);

#endif