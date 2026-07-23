#include "compiler_common.h"

#define IS_CONSTANT()                         \
    (check(compiler->parser, TOKEN_STRING) || \
     check(compiler->parser, TOKEN_NUMBER))

static void getCommand(Compiler* compiler) {
    emitByte(compiler, OP_GET);
    parseKey(compiler, "expect a key after 'GET'");
}

static void setCommand(Compiler* compiler) {
    emitByte(compiler, OP_SET);
    parseKey(compiler, "expect a key after 'SET'");
    parseValue(compiler, "expect a value after key");
}

static void delCommand(Compiler* compiler) {
    emitByte(compiler, OP_DEL);
    parseKey(compiler, "expect a key after 'DEL'");
}

static void keysCommand(Compiler* compiler) {
    emitByte(compiler, OP_KEYS);
    // TODO: need to take in a string as pattern and need to emit it
}

static void existsCommand(Compiler* compiler) {
    emitByte(compiler, OP_EXISTS);
    int offset = emitPlaceHolderByte(compiler);
    uint8_t count = 0;
    while (check(compiler->parser, TOKEN_STRING)) {
        if (count == UINT8_MAX) {
            error(compiler, "Too many arguments.");
        }
        parseKey(compiler, "Expect a key.");
        count++;
    }
    if (count == 0) {
        error(compiler, "EXISTS expects at least 1 key");
    }
    patchByte(compiler, offset, count);
}
static void typeCommand(Compiler* compiler) {
    emitByte(compiler, OP_TYPE);
    parseKey(compiler, "expect a key after 'TYPE'");
}

static void DBSizeCommand(Compiler* compiler) { emitByte(compiler, OP_DBSIZE); }
static void flushDBCommand(Compiler* compiler) { emitByte(compiler, OP_FLUSH); }

static void exitCommand(Compiler* compiler) { emitByte(compiler, OP_EXIT); }
static void quitCommand(Compiler* compiler) { emitByte(compiler, OP_EXIT); }

static void pingCommand(Compiler* compiler) { emitByte(compiler, OP_PING); }

CommandFn commandTable[] = {
    [TOKEN_GET] = getCommand,       [TOKEN_SET] = setCommand,
    [TOKEN_DEL] = delCommand,       [TOKEN_KEYS] = keysCommand,
    [TOKEN_EXIT] = exitCommand,     [TOKEN_PING] = pingCommand,
    [TOKEN_QUIT] = quitCommand,     [TOKEN_DBSIZE] = DBSizeCommand,
    [TOKEN_FLUSH] = flushDBCommand, [TOKEN_TYPE] = typeCommand,
    [TOKEN_EXISTS] = existsCommand,
};

const size_t commandTableSize = sizeof(commandTable) / sizeof(commandTable[0]);