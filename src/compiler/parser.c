#include "compiler_common.h"

#define IS_COMMAND(type) \
    ((type) > TOKEN_COMMANDS_START && (type) < TOKEN_COMMANDS_END)

void initParser(Parser* parser) {
    parser->hadError = false;
    parser->panicMode = false;
}

/*
    Panic mode suppresses cascading compiler errors.

    Once synchronization is reached, normal parsing resumes.
*/
void synchronize(Compiler* compiler) {
    compiler->parser->panicMode = false;

    while (compiler->parser->current.type != TOKEN_EOF) {
        if (IS_COMMAND(compiler->parser->current.type)) {
            return;
        }

        advance(compiler);
    }
}

void advance(Compiler* compiler) {
    compiler->parser->previous = compiler->parser->current;
    for (;;) {
        compiler->parser->current = scanToken();
        if (compiler->parser->current.type != TOKEN_ERROR) break;
        errorAtCurrent(compiler, compiler->parser->current.start);
    }
}

void consume(Compiler* compiler, TokenType type, const char* message) {
    if (compiler->parser->current.type == type) {
        advance(compiler);
        return;
    }
    errorAtCurrent(compiler, message);
}

bool check(Parser* parser, TokenType type) {
    return parser->current.type == type;
}

bool match(Compiler* compiler, TokenType type) {
    if (!check(compiler->parser, type)) return false;
    advance(compiler);
    return true;
}
