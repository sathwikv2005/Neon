#ifndef neon_compiler_h
#define neon_compiler_h

#include "chunk.h"
#include "parser.h"
#include "vm.h"

typedef struct {
    Parser* parser;
    Chunk* chunk;
    VM* vm;
} Compiler;

// REPL
extern Chunk* chunk;
bool compile(VM* vm, const char* source, Chunk* chunk);
void markCompilerRoots();

// RESP
bool resp_compile(VM* vm, const char* source, Chunk* chunk);

#endif