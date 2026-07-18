#include "scanner.h"

#include <stdio.h>
#include <string.h>

typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

static Scanner scanner;

void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static bool isDigit(char c) { return c >= '0' && c <= '9'; }

static bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;

    return token;
}

static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.line = scanner.line;
    token.length = (int)(scanner.current - scanner.start);
    return token;
}

static bool isAtEnd() { return *scanner.current == '\0'; }

static char advance() {
    scanner.current++;
    return scanner.current[-1];
}

static bool match(char expected) {
    if (isAtEnd()) return false;
    if (*scanner.current != expected) return false;
    scanner.current++;
    return true;
}

static char peek() { return *scanner.current; }

static char peekNext() {
    if (isAtEnd()) return '\0';
    return scanner.current[1];
}

static void skipWhitespace() {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                scanner.line++;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                    break;
                } else {
                    return;
                }
            default:
                return;
        }
    }
}

static Token number() {
    while (isDigit(peek())) advance();

    if (peek() == '.' && isDigit(peekNext())) {
        advance();

        while (isDigit(peek())) advance();
    }

    return makeToken(TOKEN_NUMBER);
}

static Token string(char quote) {
    scanner.start++;  // skip opening quote

    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\\') {
            advance();
            if (!isAtEnd()) advance();
            continue;
        }
        if (peek() == '\n') scanner.line++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    Token token = makeToken(TOKEN_STRING);

    advance();  // consume closing quote

    return token;
}

static TokenType checkKeyword(int start, int length, const char* rest,
                              TokenType type) {
    if (scanner.current - scanner.start == start + length &&
        memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    }
    // Anything not a number of keyword is a string
    return TOKEN_STRING;
}

static TokenType wordType() {
    switch (scanner.start[0]) {
        case 'D':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'E':
                        return checkKeyword(2, 1, "L", TOKEN_DEL);
                    case 'B':
                        return checkKeyword(2, 4, "SIZE", TOKEN_DBSIZE);
                }
            }
            break;
        case 'E':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'C':
                        return checkKeyword(2, 2, "HO", TOKEN_ECHO);
                    case 'X':
                        if (scanner.current - scanner.start == 4)
                            return checkKeyword(2, 2, "IT", TOKEN_EXIT);
                        return checkKeyword(2, 4, "ISTS", TOKEN_EXISTS);
                }
            }
            break;
        case 'F':
            return checkKeyword(1, 6, "LUSHDB", TOKEN_FLUSH);
        case 'G':
            return checkKeyword(1, 2, "ET", TOKEN_GET);
        case 'K':
            return checkKeyword(1, 4, "EYS", TOKEN_KEYS);
        case 'P':
            return checkKeyword(1, 4, "ING", TOKEN_PING);
        case 'R':
            return checkKeyword(1, 5, "ENAME", TOKEN_RENAME);
        case 'S':
            return checkKeyword(1, 2, "ET", TOKEN_SET);
        case 'T':
            return checkKeyword(1, 3, "YPE", TOKEN_TYPE);
    }
    // Anything not a number of keyword is a string
    return TOKEN_STRING;
}

static Token word() {
    while (!isAtEnd()) {
        char c = peek();

        if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '"' ||
            c == '\'' || c == ';')
            break;

        advance();
    }
    return makeToken(wordType());
}

Token scanToken() {
    skipWhitespace();
    scanner.start = scanner.current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();

    if (isDigit(c)) return number();
    if (isAlpha(c)) return word();
    if (c == '"' || c == '\'') return string(c);

    return errorToken("Unexpected character.");
}