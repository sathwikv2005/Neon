#include "compiler.h"
#include "engine_common.h"

Engine engine;

void initEngine() {
    resetStack();
    engine.objects = NULL;
    engine.grayCount = 0;
    engine.grayCapacity = 0;
    engine.grayStack = NULL;
    engine.debugFlags = 0;
    engine.atLineStart = true;
    engine.bytesAllocated = 0;
    engine.nextGC = 1024 * 1024;
    engine.currentGCMark = true;
    engine.ip = 0;
    // initTable(&engine.strings);
}

void freeEngine() {
    freeObjects();
    // freeTable(&engine.strings);
}

static InterpretResult run() {
    Chunk* chunk = &engine.chunk;
    register uint8_t* ip = engine.ip;

/*
    The ip is cached locally so, that the c compiler can store it in a register
    for faster access hence, the ip must be written back to the call frame so
    that the runtimeError() can report the correct source line.
*/
#define RUNTIME_ERROR(...)         \
    do {                           \
        engine.ip = ip;            \
        runtimeError(__VA_ARGS__); \
    } while (false)

#define READ_BYTE() (*ip++)
#define READ_CONSTANT() (chunk.constants.values[READ_BYTE()])
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
        disassembleInstruction(
            &frame->closure->function->chunk,
            (int)(ip - frame->closure->function->chunk.code));
        // printf("==+++++++++==\n");
    }
#endif
    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
        // op codes
    }
}

InterpretResult interpret(const char* source) {
    switch (setjmp(engine.engineJmp)) {
        case JUMP_RUNTIME_ERROR:
            return INTERPRET_RUNTIME_ERROR;
        case JUMP_EXIT:
            return INTERPRET_EXIT;
    }
    const Chunk* chunk = compile(source);
    engine.chunk = chunk;
    engine.ip = chunk->code;
    return run();
}