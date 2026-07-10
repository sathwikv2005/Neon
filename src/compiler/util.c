#include <stdio.h>

#include "compiler_common.h"

void errorAt(Compiler* compiler, Token* token, const char* message) {
    if (compiler->parser->panicMode) return;
    compiler->parser->panicMode = true;
    fprintf(stderr, ANSI_RED "Compiler Error: " ANSI_RESET);
    fprintf(stderr, ANSI_YELLOW "%d:" ANSI_RESET, token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, ANSI_DIM " at " ANSI_RESET ANSI_BOLD ANSI_CYAN
                                 "end" ANSI_RESET);
    } else if (token->type == TOKEN_ERROR) {
        // nothing
    } else {
        fprintf(stderr,
                ANSI_DIM " at " ANSI_RESET ANSI_BOLD ANSI_CYAN
                         "'%.*s'" ANSI_RESET,
                token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    compiler->parser->hadError = true;
}

void error(Compiler* compiler, const char* message) {
    //
    errorAt(compiler, &compiler->parser->previous, message);
}

void errorAtCurrent(Compiler* compiler, const char* message) {
    errorAt(compiler, &compiler->parser->current, message);
}

uint8_t makeConstant(Compiler* compiler, Value value) {
    int constant = addConstant(currentChunk(compiler), value);
    if (constant > UINT8_MAX) {
        error(compiler, "Too many constants in one chunk.");
        return 0;
    }
    return (uint8_t)constant;
}

void emitByte(Compiler* compiler, uint8_t byte) {
    if (compiler->parser->hadError) return;
    writeChunk(currentChunk(compiler), byte, compiler->parser->previous.line);
}
void emitBytes(Compiler* compiler, uint8_t byte1, uint8_t byte2) {
    emitByte(compiler, byte1);
    emitByte(compiler, byte2);
}