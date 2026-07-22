#include "compiler_common.h"

void getCommand(Compiler* compiler) {
    emitByte(compiler, OP_GET);
    parseKey(compiler, "expect a key after 'GET'");
}

void setCommand(Compiler* compiler) {
    emitByte(compiler, OP_SET);
    parseKey(compiler, "expect a key after 'SET'");
    parseValue(compiler, "expect a value after key");
}

void delCommand(Compiler* compiler) {
    emitByte(compiler, OP_DEL);
    parseKey(compiler, "expect a key after 'DEL'");
}

void keysCommand(Compiler* compiler) {
    emitByte(compiler, OP_KEYS);
    // TODO: need to take in a string as pattern and need to emit it
}

void pingCommand(Compiler* compiler) { emitByte(compiler, OP_PING); }