#include <stdarg.h>
#include <stdio.h>

#include "vm_common.h"

static void vsetError(VM* vm, const char* fmt, va_list args) {
    va_list copy;
    va_copy(copy, args);

    int length = vsnprintf(NULL, 0, fmt, copy);
    va_end(copy);

    if (length < 0) return;

    size_t required = (size_t)length + 1;

    if (required > vm->errorCapacity) {
        vm->error = GROW_ARRAY(char, vm->error, vm->errorCapacity, required);
        vm->errorCapacity = required;
    }

    vsnprintf(vm->error, vm->errorCapacity, fmt, args);
}

void setError(VM* vm, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsetError(vm, fmt, args);
    va_end(args);
}

void runtimeError(VM* vm, const char* format, ...) {
    va_list args;
    va_start(args, format);

    vsetError(vm, format, args);

    va_end(args);

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
