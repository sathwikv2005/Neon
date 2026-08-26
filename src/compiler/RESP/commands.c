#include <string.h>

#include "chunk.h"
#include "compiler.h"
#include "object.h"
#include "resp_common.h"

static void compilerError(Compiler* compiler, const char* message) {
    compiler->parser->hadError = true;
    error(compiler, message);
}

static bool isString(const RespValue* value) {
    return value->type == RESP_BULK_STRING || value->type == RESP_SIMPLE_STRING;
}

bool commandEquals(const RespValue* value, const char* command) {
    if (!isString(value)) return false;

    size_t length = strlen(command);

    if (value->string.length != length) return false;

    for (size_t i = 0; i < length; i++) {
        char a = value->string.data[i];
        char b = command[i];

        if (a >= 'a' && a <= 'z') a -= 'a' - 'A';

        if (a != b) return false;
    }

    return true;
}

bool compileGet(Compiler* compiler, const RespValue* request) {
    if (request->array.count != 2) {
        compilerError(compiler, "wrong number of arguments for 'get' command");
        return false;
    }

    if (!isString(&request->array.values[1])) {
        compilerError(compiler, "invalid argument for 'get' command");
        return false;
    }

    ObjString* key = copyString(compiler->parser->previous.start,
                                compiler->parser->previous.length);

    uint8_t constant = makeConstant(compiler, OBJ_VAL(key));

    emitBytes(compiler, OP_GET, constant);

    return true;
}

bool compileCommand(Compiler* compiler, const RespValue* request) {
    if (request->type != RESP_ARRAY) {
        error(compiler, "command must be an array");
        return false;
    }

    if (request->array.count == 0) {
        error(compiler, "empty command");
        return false;
    }

    const RespValue* command = &request->array.values[0];

    if (!isString(command)) {
        error(compiler, "command name must be a string");
        return false;
    }

    if (commandEquals(command, "GET")) {
        return compileGet(compiler, request);
    }

    error(compiler, "unknown command");
    return false;
}