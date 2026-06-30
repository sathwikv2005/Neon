#include "compiler_common.h"

static Chunk compilingChunk;
Chunk* chunk = &compilingChunk;

static void command() {}

Chunk* compile(const char* source) {
    initScanner(source);

    initChunk(chunk);
    parser.hadError = false;
    parser.panicMode = false;

    advance();

    while (!match(TOKEN_EOF)) {
        command();
    }
    if (parser.hadError) return NULL;

    return chunk;
}

void markCompilerRoots() {
    // nothing
}