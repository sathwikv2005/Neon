#ifndef neon_compiler_h
#define neon_compiler_h

#include "chunk.h"

extern Chunk* chunk;
Chunk* compile(const char* source);
void markCompilerRoots();

#endif