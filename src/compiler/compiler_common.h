#ifndef neon_compiler_common_h
#define neon_compiler_common_h

#include "compiler.h"
#include "object.h"
#include "parser.h"
#include "repl_scanner.h"
#include "value.h"

// util
void errorAt(Compiler* compiler, Token* token, const char* fmt, ...);
void error(Compiler* compiler, const char* message);
void errorAtCurrent(Compiler* compiler, const char* message);
uint8_t makeConstant(Compiler* compiler, Value value);
void emitByte(Compiler* compiler, uint8_t byte);
void emitBytes(Compiler* compiler, uint8_t byte1, uint8_t byte2);
int emitPlaceHolderByte(Compiler* compiler);
void patchByte(Compiler* compiler, int offset, uint8_t newByte);

#endif