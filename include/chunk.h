#ifndef neon_chunk_h
#define neon_chunk_h

#include "line.h"
#include "value.h"

typedef struct {
    int count;
    int capacity;
    uint8_t* code;         /**< Bytecode array */
    LineArray lines;       // compressed line info
    ValueArray constants;  // constant values storage

} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value value);
int getLine(const Chunk* chunk, int offset);

#endif