#include <stdio.h>

#include "compiler.h"
#include "debug.h"
#include "vm_common.h"

// return ok back to the engine
#define INTERPRET_OK() ((InterpretOutput){INTERPRET_OK, false, NULL_VAL})

// return ok with a result back to the engine
#define INTERPRET_RESULT(v) ((InterpretOutput){INTERPRET_OK, true, (v)})

// report an error back to the engine
#define ERROR_STATUS(s) ((InterpretOutput){(s), false, NULL_VAL})

void initvm(VM* vm) {
    resetStack(vm);
    vm->objects = NULL;
    // vm->grayCount = 0;
    // vm->grayCapacity = 0;
    // vm->grayStack = NULL;
    vm->debugFlags = 0;
    vm->atLineStart = true;
    vm->bytesAllocated = 0;
    // vm->nextGC = 1024 * 1024;
    // vm->currentGCMark = true;
    vm->ip = 0;
    vm->error = NULL;
    vm->errorCapacity = 0;
}

void freevm(VM* vm) {
    FREE_ARRAY(char, vm->error);
    vm->error = NULL;
    vm->errorCapacity = 0;
    freeObjects(vm);
}

static InterpretOutput run(VM* vm) {
    const Chunk* chunk = vm->chunk;
    register uint8_t* ip = vm->ip;

/*
    The ip is cached locally so, that the c compiler can store it in a register
    for faster access hence, the ip must be written back to the call frame so
    that the runtimeError() can report the correct source line.
*/
#define RUNTIME_ERROR(...)             \
    do {                               \
        vm->ip = ip;                   \
        runtimeError(vm, __VA_ARGS__); \
    } while (false)
#define PUSH(v) push(vm, (v))
#define POP() pop(vm)
#define PEEK(n) peek(vm, (n))
#define READ_BYTE() (*ip++)
#define READ_CONSTANT() (chunk->constants.values[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())

    while (true) {
#ifdef NEON_DEBUG
        if (GET_DEBUG_TRACE()) {
            printf("Stack=>\t");
            printf("[ ");
            for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
                printValue(*slot);
                printf(", ");
            }
            printf("]");
            printf("\n");
            disassembleInstruction(chunk, (int)(ip - chunk->code));
            // printf("==+++++++++==\n");
        }
#endif
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_SET: {
                ObjString* key = READ_STRING();
                Value value = READ_CONSTANT();
                tableSet(&vm->database->table, key, value);
                return INTERPRET_OK();
            }

            case OP_GET: {
                ObjString* key = READ_STRING();
                Value value;
                if (!tableGet(&vm->database->table, key, &value)) {
                    return INTERPRET_RESULT(NULL_VAL);
                }
                return INTERPRET_RESULT(value);
            }

            case OP_RETURN:
                return INTERPRET_OK();
            default:
                break;
        }
    }
    return ERROR_STATUS(INTERPRET_RUNTIME_ERROR);
}

InterpretOutput interpret(const char* source, VM* vm) {
    switch (setjmp(vm->vmJmp)) {
        case JUMP_RUNTIME_ERROR:
            return ERROR_STATUS(INTERPRET_RUNTIME_ERROR);
        case JUMP_EXIT:
            return ERROR_STATUS(INTERPRET_EXIT);
    }
    Chunk chunk;
    initChunk(&chunk);
    if (!compile(vm, source, &chunk)) {
        return ERROR_STATUS(INTERPRET_COMPILE_ERROR);
    }

    vm->chunk = &chunk;
    vm->ip = chunk.code;

    InterpretOutput result = run(vm);

    freeChunk(&chunk);

    return result;
}