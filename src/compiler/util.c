#include <stdarg.h>
#include <stdio.h>

#include "compiler_common.h"

static void verrorAt(Compiler* compiler, Token* token, const char* fmt,
                     va_list args) {
    if (compiler->parser->panicMode) return;
    compiler->parser->panicMode = true;

    VM* vm = compiler->vm;

    char message[1024];
    vsnprintf(message, sizeof(message), fmt, args);

    if (token->type == TOKEN_EOF) {
        setError(vm, "Compiler Error %d at end: %s", token->line, message);
    } else if (token->type == TOKEN_ERROR) {
        setError(vm, "Compiler Error %d: %s", token->line, message);
    } else {
        setError(vm, "Compiler Error %d at '%.*s': %s", token->line,
                 token->length, token->start, message);
    }

    compiler->parser->hadError = true;
}

void errorAt(Compiler* compiler, Token* token, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    verrorAt(compiler, token, fmt, args);
    va_end(args);
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