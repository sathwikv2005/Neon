#ifndef neon_compiler_common_h
#define neon_compiler_common_h

#include "compiler.h"
#include "object.h"
#include "parser.h"
#include "scanner.h"
#include "value.h"

// util
void errorAt(Compiler* compiler, Token* token, const char* fmt, ...);
void error(Compiler* compiler, const char* message);
void errorAtCurrent(Compiler* compiler, const char* message);
uint8_t makeConstant(Compiler* compiler, Value value);
void emitByte(Compiler* compiler, uint8_t byte);
void emitBytes(Compiler* compiler, uint8_t byte1, uint8_t byte2);

// parser
void synchronize(Compiler* compiler);
void advance(Compiler* compiler);
void consume(Compiler* compiler, TokenType type, const char* message);
bool check(Parser* parser, TokenType type);
bool match(Compiler* compiler, TokenType type);

// compiler
void parseKey(Compiler* compiler, const char* message);
void parseValue(Compiler* compiler, const char* message);
Chunk* currentChunk(Compiler* compiler);

// commands
void getCommand(Compiler* compiler);
void setCommand(Compiler* compiler);
void delCommand(Compiler* compiler);
void keysCommand(Compiler* compiler);
void pingCommand(Compiler* compiler);

#endif