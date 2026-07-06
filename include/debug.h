#ifndef neon_debug_h
#define neon_debug_h

#include "chunk.h"

void disassembleChunk(const Chunk* chunk, const char* name);
int disassembleInstruction(const Chunk* chunk, int offset);

#endif