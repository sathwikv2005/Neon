#include <stdio.h>

#include "engine_common.h"

void runtimeError(const char* format, ...) {
    if (!engine.atLineStart) putchar('\n');
    fprintf(stderr, ANSI_RED "Runtime Error: " ANSI_RESET);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    resetStack();
    longjmp(engine.engineJmp, JUMP_RUNTIME_ERROR);
}

Value peek(int distance) { return engine.stackTop[-1 - distance]; }

void push(Value value) {
    if (engine.stackTop == engine.stack + STACK_MAX) {
        runtimeError("Stack overflow error.");
    }
    *engine.stackTop++ = value;
}

Value pop() {
    if (engine.stackTop == engine.stack) {
        runtimeError("Stack underflow error.");
    }
    return *--engine.stackTop;
}

bool isFalsey(Value value) {
    return IS_NULL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

void concatenate() {
    ObjString* b = AS_STRING(valueToString(peek(0)));
    ObjString* a = AS_STRING(valueToString(peek(1)));

    int length = a->length + b->length;
    char* chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    ObjString* result = takeString(chars, length);
    pop();
    pop();
    push(OBJ_VAL(result));
}

void resetStack() { engine.stackTop = engine.stack; }