#include "resp_common.h"

static void initCompiler(Compiler* compiler, VM* vm, Parser* parser,
                         Chunk* chunk) {
    compiler->vm = vm;
    compiler->parser = parser;
    compiler->chunk = chunk;
}

bool resp_compile(VM* vm, const char* source, Chunk* chunk) {
    Compiler compiler;

    Parser parser;
    initParser(&parser);

    initCompiler(&compiler, vm, &parser, chunk);

    RespValue request;

    RespParseResult result = parseResp(source, strlen(source), &request);

    if (result == RESP_PARSE_INCOMPLETE) {
        error(&compiler, "incomplete RESP request");

        freeRespValue(&request);
        return false;
    }

    if (result == RESP_PARSE_ERROR) {
        error(&compiler, "invalid RESP request");

        freeRespValue(&request);
        return false;
    }

    bool success = compileCommand(&compiler, &request);

    if (success) {
        emitByte(&compiler, OP_RETURN);
    }

#ifdef NEON_DEBUG
    if (!compiler.parser->hadError) {
        disassembleChunk(chunk, "<resp>");
    }
#endif

    freeRespValue(&request);

    return success;
}