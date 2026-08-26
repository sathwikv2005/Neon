#ifndef NEON_REPL_COMMON_H
#define NEON_REPL_COMMON_H

#include "../compiler_common.h"

// parser
void synchronize(Compiler* compiler);
void advance(Compiler* compiler);
void consume(Compiler* compiler, NeonTokenType type, const char* message);
bool check(Parser* parser, NeonTokenType type);
bool match(Compiler* compiler, NeonTokenType type);

// compiler
void parseKey(Compiler* compiler, const char* message);
void parseValue(Compiler* compiler, const char* message);
Chunk* currentChunk(Compiler* compiler);

// commands
typedef void (*CommandFn)(Compiler*);
extern CommandFn commandTable[];
extern const size_t commandTableSize;

#endif