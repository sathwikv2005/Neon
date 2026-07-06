#include <stdio.h>

#include "compiler.h"
#include "debug.h"
#include "vm_common.h"

VM vm;

#define ERROR_STATUS(status) \
    (InterpretOutput) { status, NULL_VAL }

void initvm() {
    resetStack();
    vm.objects = NULL;
    vm.grayCount = 0;
    vm.grayCapacity = 0;
    vm.grayStack = NULL;
    vm.debugFlags = 0;
    vm.atLineStart = true;
    vm.bytesAllocated = 0;
    vm.nextGC = 1024 * 1024;
    vm.currentGCMark = true;
    vm.ip = 0;
    initTable(&vm.strings);
}

void freevm() {
    freeObjects();
    freeTable(&vm.strings);
}

static InterpretOutput run() {
    const Chunk* chunk = vm.chunk;
    register uint8_t* ip = vm.ip;

/*
    The ip is cached locally so, that the c compiler can store it in a register
    for faster access hence, the ip must be written back to the call frame so
    that the runtimeError() can report the correct source line.
*/
#define RUNTIME_ERROR(...)         \
    do {                           \
        vm.ip = ip;                \
        runtimeError(__VA_ARGS__); \
    } while (false)

#define READ_BYTE() (*ip++)
#define READ_CONSTANT() (chunk.constants.values[READ_BYTE()])

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
            // op codes
        }
    }
}

InterpretOutput interpret(const char* source) {
    switch (setjmp(vm.vmJmp)) {
        case JUMP_RUNTIME_ERROR:
            return ERROR_STATUS(INTERPRET_RUNTIME_ERROR);
        case JUMP_EXIT:
            return ERROR_STATUS(INTERPRET_EXIT);
    }
    Chunk* chunk = compile(source);
    if (chunk == NULL) {
        return ERROR_STATUS(INTERPRET_COMPILE_ERROR);
    }

    vm.chunk = chunk;
    vm.ip = chunk->code;

    InterpretOutput result = run();

    freeChunk(chunk);

    return result;
}