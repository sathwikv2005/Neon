#include <stdio.h>

#include "compiler_common.h"

void errorAt(Token* token, const char* message) {
    if (parser.panicMode) return;
    parser.panicMode = true;
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
    parser.hadError = true;
}

void error(const char* message) {
    //
    errorAt(&parser.previous, message);
}

void errorAtCurrent(const char* message) { errorAt(&parser.current, message); }

uint8_t makeConstant(Value value) {
    int constant = addConstant(chunk, value);
    if (constant > UINT8_MAX) {
        error("Too many constants in one chunk.");
        return 0;
    }
    return (uint8_t)constant;
}