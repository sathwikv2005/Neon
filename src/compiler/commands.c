#include "compiler_common.h"

void getCommand() {
    emitByte(OP_GET);
    parseKey("expect a key after 'GET'");
}

void setCommand() {
    emitByte(OP_SET);
    parseKey("expect a key after 'SET'");
    parseValue("expect a value after key");
}