#ifndef neon_compiler_h
#define neon_compiler_h

#include "chunk.h"
#include "parser.h"
#include "scanner.h"

extern Chunk* chunk;
Chunk* compile(const char* source);
void markCompilerRoots();

#endif