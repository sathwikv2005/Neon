#include "compiler_common.h"
#include "debug.h"
#include "stdlib.h"

static Chunk compilingChunk;
Chunk* chunk = &compilingChunk;

Chunk* currentChunk() { return chunk; }

void parseKey(const char* message) {
    consume(TOKEN_STRING, message);
    ObjString* key = copyString(parser.previous.start, parser.previous.length);

    uint8_t constant = makeConstant(OBJ_VAL(key));
    emitByte(constant);
}

void parseValue(const char* message) {
    uint8_t constant = 0;
    switch (parser.current.type) {
        case TOKEN_STRING: {
            ObjString* value =
                copyString(parser.current.start, parser.current.length);

            constant = makeConstant(OBJ_VAL(value));
            break;
        }

        case TOKEN_NUMBER: {
            double value = strtod(parser.current.start, NULL);
            constant = makeConstant(NUMBER_VAL(value));
            break;
        }
        default:
            error(message);
            return;
    }
    emitByte(constant);
    advance();
}

static void command() {
    advance();
    switch (parser.previous.type) {
        case TOKEN_GET:
            getCommand();
            break;
        case TOKEN_SET:
            setCommand();
            break;

        default:
            error("unexpected command");
            break;
    }

    // recover from panic mode.
    if (parser.panicMode) synchronize();
}

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

#ifdef NEON_DEBUG
    if (!parser.hadError) disassembleChunk(chunk, "<script>");
#endif

    return chunk;
}

void markCompilerRoots() {
    // nothing
}