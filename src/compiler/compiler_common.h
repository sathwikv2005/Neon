#ifndef neon_compiler_common_h
#define neon_compiler_common_h

#include "compiler.h"
#include "object.h"
#include "parser.h"
#include "scanner.h"
#include "value.h"

// util
void errorAt(Token* token, const char* message);
void error(const char* message);
void errorAtCurrent(const char* message);
uint8_t makeConstant(Value value);
void emitByte(uint8_t byte);
void emitBytes(uint8_t byte1, uint8_t byte2);

// parser
extern Parser parser;
void synchronize();
void advance();
void consume(TokenType type, const char* message);
bool check(TokenType type);
bool match(TokenType type);

// compiler
void parseKey(const char* message);
void parseValue(const char* message);
Chunk* currentChunk();

// commands
void getCommand();
void setCommand();

#endif