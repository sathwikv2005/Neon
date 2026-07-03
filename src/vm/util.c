#include <stdarg.h>
#include <stdio.h>

#include "vm_common.h"

void runtimeError(const char* format, ...) {
    if (!vm.atLineStart) putchar('\n');
    fprintf(stderr, ANSI_RED "Runtime Error: " ANSI_RESET);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    resetStack();
    longjmp(vm.vmJmp, JUMP_RUNTIME_ERROR);
}

Value peek(int distance) { return vm.stackTop[-1 - distance]; }

void push(Value value) {
    if (vm.stackTop == vm.stack + STACK_MAX) {
        runtimeError("Stack overflow error.");
    }
    *vm.stackTop++ = value;
}

Value pop() {
    if (vm.stackTop == vm.stack) {
        runtimeError("Stack underflow error.");
    }
    return *--vm.stackTop;
}

bool isFalsey(Value value) { return IS_NULL(value); }

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

void resetStack() { vm.stackTop = vm.stack; }