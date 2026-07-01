#ifndef neon_engine_h
#define neon_engine_h

#include <setjmp.h>

#include "chunk.h"
#include "common.h"
#include "object.h"
#include "value.h"

// debug macros
#define DEBUG_TRACE (1u << 0)
#define DEBUG_CODE (1u << 1)
#define DEBUG_STRESS_GC (1u << 2)
#define DEBUG_LOG_GC (1u << 3)

// setters
#define SET_DEBUG_TRACE() (vm.debugFlags |= DEBUG_TRACE)
#define SET_DEBUG_CODE() (vm.debugFlags |= DEBUG_CODE)
#define SET_DEBUG_STRESS_GC() (vm.debugFlags |= DEBUG_STRESS_GC)
#define SET_DEBUG_LOG_GC() (vm.debugFlags |= DEBUG_LOG_GC)

// enable all
#define SET_DEBUG()            \
    do {                       \
        SET_DEBUG_CODE();      \
        SET_DEBUG_TRACE();     \
        SET_DEBUG_STRESS_GC(); \
        SET_DEBUG_LOG_GC();    \
    } while (0)

// getters
#define GET_DEBUG_TRACE() (((vm.debugFlags) & DEBUG_TRACE) != 0)
#define GET_DEBUG_CODE() (((vm.debugFlags) & DEBUG_CODE) != 0)
#define GET_DEBUG_STRESS_GC() (((vm.debugFlags) & DEBUG_STRESS_GC) != 0)
#define GET_DEBUG_LOG_GC() (((vm.debugFlags) & DEBUG_LOG_GC) != 0)

#define STACK_MAX UINT8_COUNT

typedef enum { JUMP_RUNTIME_ERROR = 1, JUMP_EXIT = 2 } JumpReason;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
    INTERPRET_EXIT
} InterpretResult;
typedef struct {
    Value stack[STACK_MAX];
    Chunk* chunk;
    // gc
    size_t bytesAllocated;
    size_t nextGC;
    Obj* objects;
    int grayCount;
    int grayCapacity;
    Obj** grayStack;
    bool currentGCMark;

    /*
        Invariant:
        stackTop always points one past the last occupied stack slot.
    */
    Value* stackTop;

    uint8_t* ip;

    int exitCode;
    jmp_buf engineJmp;

    bool atLineStart;

    // debug flags
    uint8_t debugFlags;
} Engine;

extern Engine engine;

void push(Value value);
Value pop();
bool isFalsey(Value value);
void runtimeError(const char* format, ...);

InterpretResult interpret(const char* source);

#endif