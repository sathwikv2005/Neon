#include "../include/chunk.h"

#include <stdlib.h>

#include "../include/line.h"
#include "../include/memory.h"
#include "../include/value.h"

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    initLineArray(&chunk->lines);
    initValueArray(&chunk->constants);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code =
            GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }
    chunk->code[chunk->count] = byte;
    chunk->count++;

    LineArray* lines = &chunk->lines;

    if (lines->count > 0 && lines->runs[lines->count - 1].line == line) {
        lines->runs[lines->count - 1].count++;
    } else {
        if (lines->capacity < lines->count + 1) {
            int oldCapacity = lines->capacity;
            lines->capacity = GROW_CAPACITY(oldCapacity);
            lines->runs =
                GROW_ARRAY(LineRun, lines->runs, oldCapacity, lines->capacity);
        }

        lines->runs[lines->count].line = line;
        lines->runs[lines->count].count = 1;
        lines->count++;
    }
}

/*
    objects other than strings only exist in the scope of that single command.
    so they survive only if they make it to a table.
*/
static void freeConstants(ValueArray* constants) {
    for (int i = 0; i < constants->count; i++) {
        if (!IS_OBJ(constants->values[i])) continue;
        Obj* obj = AS_OBJ(constants->values[i]);
        if (!obj->survived) {
            releaseObject(obj);
        }
    }
}

void freeChunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    freeLineArray(&chunk->lines);
    freeConstants(&chunk->constants);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

int addConstant(Chunk* chunk, Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}

int getLine(const Chunk* chunk, int offset) {
    int accumulated = 0;

    for (int i = 0; i < chunk->lines.count; i++) {
        accumulated += chunk->lines.runs[i].count;
        if (offset < accumulated) {
            return chunk->lines.runs[i].line;
        }
    }

    return -1;
}