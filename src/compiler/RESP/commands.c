#include <stdint.h>
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

static bool isNumber(const RespValue* value) {
    return value->type == RESP_INTEGER;
}

static bool parseNumber(const RespValue* value, double* number) {
    if (!isString(value)) return false;

    char buffer[64];

    if (value->string.length >= sizeof(buffer)) {
        return false;
    }

    memcpy(buffer, value->string.data, value->string.length);
    buffer[value->string.length] = '\0';

    char* end;
    errno = 0;

    *number = strtod(buffer, &end);

    return end != buffer && *end == '\0' && errno != ERANGE;
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

/*
 * Convert a RESP string into a VM string constant.
 */
static uint8_t makeStringConstant(Compiler* compiler, const RespValue* value) {
    ObjString* string = copyString(value->string.data, value->string.length);

    return makeConstant(compiler, OBJ_VAL(string));
}

/*
 * Validate that an argument is a string and emit it as a constant.
 */
static bool emitStringArgument(Compiler* compiler, const RespValue* value,
                               const char* errorMessage) {
    if (!isString(value)) {
        compilerError(compiler, errorMessage);
        return false;
    }

    uint8_t constant = makeStringConstant(compiler, value);
    emitByte(compiler, constant);

    return true;
}

/*
 * Validate that an argument is a string and emit it as the correct value type.
 */
static bool emitValueArgument(Compiler* compiler, const RespValue* value,
                              const char* errorMessage) {
    if (!isString(value)) {
        compilerError(compiler, errorMessage);
        return false;
    }

    char buffer[64];

    if (value->string.length < sizeof(buffer)) {
        memcpy(buffer, value->string.data, value->string.length);
        buffer[value->string.length] = '\0';

        char* end;
        errno = 0;

        double number = strtod(buffer, &end);

        /*
         * Numeric only if the entire argument was consumed.
         *
         * "123"   -> number
         * "12.5"  -> number
         * "-10"   -> number
         * "123a"  -> string
         * "abc"   -> string
         */
        if (end != buffer && *end == '\0' && errno != ERANGE) {
            uint8_t constant = makeConstant(compiler, NUMBER_VAL(number));

            emitByte(compiler, constant);
            return true;
        }
    }

    uint8_t constant = makeStringConstant(compiler, value);
    emitByte(compiler, constant);

    return true;
}

/*
 * Validate the exact number of arguments.
 *
 * request->array.count includes the command itself.
 */
static bool checkArgCount(Compiler* compiler, const RespValue* request,
                          size_t expected, const char* command) {
    if (request->array.count != expected) {
        compilerError(compiler, command);
        return false;
    }

    return true;
}

static bool compileGet(Compiler* compiler, const RespValue* request) {
    if (!checkArgCount(compiler, request, 2,
                       "wrong number of arguments for 'get' command")) {
        return false;
    }

    emitByte(compiler, OP_GET);

    return emitStringArgument(compiler, &request->array.values[1],
                              "invalid argument for 'get' command");
}

static bool compileSet(Compiler* compiler, const RespValue* request) {
    if (!checkArgCount(compiler, request, 3,
                       "wrong number of arguments for 'set' command")) {
        return false;
    }

    const RespValue* key = &request->array.values[1];
    const RespValue* value = &request->array.values[2];

    emitByte(compiler, OP_SET);

    if (!emitStringArgument(compiler, key, "invalid key for 'set' command")) {
        return false;
    }

    if (!emitValueArgument(compiler, value,
                           "invalid value for 'set' command")) {
        return false;
    }

    return true;
}

static bool compileDel(Compiler* compiler, const RespValue* request) {
    if (!checkArgCount(compiler, request, 2,
                       "wrong number of arguments for 'del' command")) {
        return false;
    }

    emitByte(compiler, OP_DEL);

    return emitStringArgument(compiler, &request->array.values[1],
                              "invalid key for 'del' command");
}

static bool compileIncr(Compiler* compiler, const RespValue* request) {
    if (!checkArgCount(compiler, request, 2,
                       "wrong number of arguments for 'incr' command")) {
        return false;
    }

    emitByte(compiler, OP_INCR);

    return emitStringArgument(compiler, &request->array.values[1],
                              "invalid key for 'incr' command");
}

static bool compileDecr(Compiler* compiler, const RespValue* request) {
    if (!checkArgCount(compiler, request, 2,
                       "wrong number of arguments for 'decr' command")) {
        return false;
    }

    emitByte(compiler, OP_DECR);

    return emitStringArgument(compiler, &request->array.values[1],
                              "invalid key for 'decr' command");
}

static bool compileType(Compiler* compiler, const RespValue* request) {
    if (!checkArgCount(compiler, request, 2,
                       "wrong number of arguments for 'type' command")) {
        return false;
    }

    emitByte(compiler, OP_TYPE);

    return emitStringArgument(compiler, &request->array.values[1],
                              "invalid key for 'type' command");
}

static bool compileRename(Compiler* compiler, const RespValue* request) {
    if (!checkArgCount(compiler, request, 3,
                       "wrong number of arguments for 'rename' command")) {
        return false;
    }

    emitByte(compiler, OP_RENAME);

    if (!emitStringArgument(compiler, &request->array.values[1],
                            "invalid old key for 'rename' command")) {
        return false;
    }

    if (!emitStringArgument(compiler, &request->array.values[2],
                            "invalid new key for 'rename' command")) {
        return false;
    }

    return true;
}

static bool compileExists(Compiler* compiler, const RespValue* request) {
    /*
     * EXISTS key [key ...]
     *
     * At least one key is required.
     */
    if (request->array.count < 2) {
        compilerError(compiler,
                      "wrong number of arguments for 'exists' command");
        return false;
    }

    size_t count = request->array.count - 1;

    if (count > UINT8_MAX) {
        compilerError(compiler, "too many arguments for 'exists' command");
        return false;
    }

    emitByte(compiler, OP_EXISTS);

    /*
     * Emit number of keys first.
     *
     * Bytecode:
     *
     * OP_EXISTS
     * key-count
     * key-constant
     * key-constant
     * ...
     */
    emitByte(compiler, (uint8_t)count);

    for (size_t i = 0; i < count; i++) {
        if (!emitStringArgument(compiler, &request->array.values[i + 1],
                                "invalid key for 'exists' command")) {
            return false;
        }
    }

    return true;
}

static bool compileKeys(Compiler* compiler, const RespValue* request) {
    if (!checkArgCount(compiler, request, 1,
                       "wrong number of arguments for 'keys' command")) {
        return false;
    }

    emitByte(compiler, OP_KEYS);

    return true;
}

static bool compileSelect(Compiler* compiler, const RespValue* request) {
    if (!checkArgCount(compiler, request, 2,
                       "wrong number of arguments for 'select' command")) {
        return false;
    }

    const RespValue* database = &request->array.values[1];

    double id;

    if (!parseNumber(database, &id)) {
        compilerError(compiler, "invalid database id for 'select' command");
        return false;
    }

    emitByte(compiler, OP_SELECT);

    uint8_t constant = makeConstant(compiler, NUMBER_VAL(id));

    emitByte(compiler, constant);

    return true;
}

static bool compileDBSize(Compiler* compiler, const RespValue* request) {
    if (!checkArgCount(compiler, request, 1,
                       "wrong number of arguments for 'dbsize' command")) {
        return false;
    }

    emitByte(compiler, OP_DBSIZE);
    return true;
}

static bool compileFlushDB(Compiler* compiler, const RespValue* request) {
    if (!checkArgCount(compiler, request, 1,
                       "wrong number of arguments for 'flushdb' command")) {
        return false;
    }

    emitByte(compiler, OP_FLUSH);
    return true;
}

static bool compileSave(Compiler* compiler, const RespValue* request) {
    if (!checkArgCount(compiler, request, 1,
                       "wrong number of arguments for 'save' command")) {
        return false;
    }

    emitByte(compiler, OP_SAVE);
    return true;
}

static bool compileExit(Compiler* compiler, const RespValue* request) {
    if (!checkArgCount(compiler, request, 1,
                       "wrong number of arguments for 'exit' command")) {
        return false;
    }

    emitByte(compiler, OP_EXIT);
    return true;
}

static bool compileQuit(Compiler* compiler, const RespValue* request) {
    if (!checkArgCount(compiler, request, 1,
                       "wrong number of arguments for 'quit' command")) {
        return false;
    }

    emitByte(compiler, OP_EXIT);
    return true;
}

static bool compileEcho(Compiler* compiler, const RespValue* request) {
    if (!checkArgCount(compiler, request, 2,
                       "wrong number of arguments for 'echo' command")) {
        return false;
    }

    emitByte(compiler, OP_ECHO);

    return emitStringArgument(compiler, &request->array.values[1],
                              "invalid value for 'echo' command");
}

static bool compilePing(Compiler* compiler, const RespValue* request) {
    if (!checkArgCount(compiler, request, 1,
                       "wrong number of arguments for 'ping' command")) {
        return false;
    }

    emitByte(compiler, OP_PING);
    return true;
}

static const CommandEntry commandTable[] = {
    {"GET", compileGet},       {"SET", compileSet},
    {"DEL", compileDel},       {"INCR", compileIncr},
    {"DECR", compileDecr},     {"EXISTS", compileExists},
    {"TYPE", compileType},     {"RENAME", compileRename},
    {"KEYS", compileKeys},     {"SELECT", compileSelect},
    {"DBSIZE", compileDBSize}, {"FLUSHDB", compileFlushDB},
    {"SAVE", compileSave},     {"EXIT", compileExit},
    {"QUIT", compileQuit},     {"ECHO", compileEcho},
    {"PING", compilePing},
};

static const size_t commandTableSize =
    sizeof(commandTable) / sizeof(commandTable[0]);

static CommandFn findCommand(const RespValue* command) {
    for (size_t i = 0; i < commandTableSize; i++) {
        if (commandEquals(command, commandTable[i].name)) {
            return commandTable[i].function;
        }
    }

    return NULL;
}

bool compileCommand(Compiler* compiler, const RespValue* request) {
    if (request->type != RESP_ARRAY) {
        compilerError(compiler, "command must be an array");
        return false;
    }

    if (request->array.count == 0) {
        compilerError(compiler, "empty command");
        return false;
    }

    const RespValue* command = &request->array.values[0];

    if (!isString(command)) {
        compilerError(compiler, "command name must be a string");
        return false;
    }

    CommandFn function = findCommand(command);

    if (function == NULL) {
        compilerError(compiler, "unknown command");
        return false;
    }

    return function(compiler, request);
}
