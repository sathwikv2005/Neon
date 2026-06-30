#include "compiler_common.h"

#define IS_COMMAND(type) \
    ((type) > TOKEN_COMMANDS_START && (type) < TOKEN_COMMANDS_END)

Parser parser;

/*
    Panic mode suppresses cascading compiler errors.

    Once synchronization is reached, normal parsing resumes.
*/
void synchronize() {
    parser.panicMode = false;

    while (parser.current.type != TOKEN_EOF) {
        if (IS_COMMAND(parser.current.type)) {
            return;
        }

        advance();
    }
}

void advance() {
    parser.previous = parser.current;
    for (;;) {
        parser.current = scanToken();
        if (parser.current.type != TOKEN_ERROR) break;
        errorAtCurrent(parser.current.start);
    }
}

void consume(TokenType type, const char* message) {
    if (parser.current.type == type) {
        advance();
        return;
    }
    errorAtCurrent(message);
}

bool check(TokenType type) { return parser.current.type == type; }

bool match(TokenType type) {
    if (!check(type)) return false;
    advance();
    return true;
}