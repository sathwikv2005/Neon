#ifndef neon_compiler_common_h
#define neon_compiler_common_h

#include "parser.h"
#include "scanner.h"

// util
void errorAt(Token* token, const char* message);
void error(const char* message);
void errorAtCurrent(const char* message);

// parser
extern Parser parser;
void synchronize();
void advance();
void consume(TokenType type, const char* message);
bool check(TokenType type);
bool match(TokenType type);

#endif