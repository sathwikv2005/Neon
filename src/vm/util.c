#include <stdarg.h>
#include <stdio.h>

#include "vm_common.h"

void runtimeError(VM* vm, const char* format, ...) {
    if (!vm->atLineStart) putchar('\n');
    fprintf(stderr, ANSI_RED "Runtime Error: " ANSI_RESET);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    resetStack(vm);
    longjmp(vm->vmJmp, JUMP_RUNTIME_ERROR);
}

Value peek(VM* vm, int distance) { return vm->stackTop[-1 - distance]; }

void push(VM* vm, Value value) {
    if (vm->stackTop == vm->stack + STACK_MAX) {
        runtimeError(vm, "Stack overflow error.");
    }
    *vm->stackTop++ = value;
}

Value pop(VM* vm) {
    if (vm->stackTop == vm->stack) {
        runtimeError(vm, "Stack underflow error.");
    }
    return *--vm->stackTop;
}

bool isFalsey(Value value) { return IS_NULL(value); }

void concatenate(VM* vm) {
    ObjString* b = AS_STRING(valueToString(peek(vm, 0)));
    ObjString* a = AS_STRING(valueToString(peek(vm, 1)));

    int length = a->length + b->length;
    char* chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    ObjString* result = takeString(chars, length);
    pop(vm);
    pop(vm);
    push(vm, OBJ_VAL(result));
}

void resetStack(VM* vm) { vm->stackTop = vm->stack; }