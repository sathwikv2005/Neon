#include "compiler_common.h"
#include "debug.h"
#include "stdlib.h"

Chunk* currentChunk(Compiler* compiler) { return compiler->chunk; }

void parseKey(Compiler* compiler, const char* message) {
    consume(compiler, TOKEN_STRING, message);
    ObjString* key = copyString(compiler->parser->previous.start,
                                compiler->parser->previous.length);

    uint8_t constant = makeConstant(compiler, OBJ_VAL(key));
    emitByte(compiler, constant);
}

void parseValue(Compiler* compiler, const char* message) {
    uint8_t constant = 0;
    switch (compiler->parser->current.type) {
        case TOKEN_STRING: {
            ObjString* value = copyString(compiler->parser->current.start,
                                          compiler->parser->current.length);

            constant = makeConstant(compiler, OBJ_VAL(value));
            break;
        }

        case TOKEN_NUMBER: {
            double value = strtod(compiler->parser->current.start, NULL);
            constant = makeConstant(compiler, NUMBER_VAL(value));
            break;
        }
        default:
            error(compiler, message);
            return;
    }
    emitByte(compiler, constant);
    advance(compiler);
}

static void command(Compiler* compiler) {
    advance(compiler);
    switch (compiler->parser->previous.type) {
        case TOKEN_GET:
            getCommand(compiler);
            break;
        case TOKEN_SET:
            setCommand(compiler);
            break;
        case TOKEN_DEL:
            delCommand(compiler);
            break;
        case TOKEN_KEYS:
            keysCommand(compiler);
            break;
        case TOKEN_EXIT: {
            exitCommand(compiler);
            break;
        }
        case TOKEN_PING:
            pingCommand(compiler);
            break;
        default:
            error(compiler, "unexpected command");
            break;
    }

    // recover from panic mode.
    if (compiler->parser->panicMode) synchronize(compiler);
}

static void initCompiler(Compiler* compiler, VM* vm, Parser* parser,
                         Chunk* chunk) {
    compiler->vm = vm;
    compiler->parser = parser;
    compiler->chunk = chunk;
}

bool compile(VM* vm, const char* source, Chunk* chunk) {
    initScanner(source);

    Parser parser;
    initParser(&parser);
    Compiler compiler;
    initCompiler(&compiler, vm, &parser, chunk);

    advance(&compiler);

    while (!match(&compiler, TOKEN_EOF)) {
        command(&compiler);
    }
    if (parser.hadError) return false;
    emitByte(&compiler, OP_RETURN);
#ifdef NEON_DEBUG
    if (!parser.hadError) disassembleChunk(chunk, "<script>");
#endif

    return true;
}

void markCompilerRoots() {
    // nothing
}