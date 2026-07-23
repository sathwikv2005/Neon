#include "compiler_common.h"

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

static void DBSizeCommand(Compiler* compiler) { emitByte(compiler, OP_DBSIZE); }

static void exitCommand(Compiler* compiler) { emitByte(compiler, OP_EXIT); }
static void quitCommand(Compiler* compiler) { emitByte(compiler, OP_EXIT); }

static void pingCommand(Compiler* compiler) { emitByte(compiler, OP_PING); }

CommandFn commandTable[] = {
    [TOKEN_GET] = getCommand,   [TOKEN_SET] = setCommand,
    [TOKEN_DEL] = delCommand,   [TOKEN_KEYS] = keysCommand,
    [TOKEN_EXIT] = exitCommand, [TOKEN_PING] = pingCommand,
    [TOKEN_QUIT] = quitCommand, [TOKEN_DBSIZE] = DBSizeCommand,
};

const size_t commandTableSize = sizeof(commandTable) / sizeof(commandTable[0]);