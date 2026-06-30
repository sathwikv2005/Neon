#ifndef neon_engine_h
#define neon_engine_h

#include <setjmp.h>

#include "chunk.h"
#include "common.h"
#include "object.h"
#include "value.h"

#define STACK_MAX UINT8_COUNT
typedef struct {
    Value stack[STACK_MAX];

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

    int exitCode;
    jmp_buf vmJump;

    bool atLineStart;

    // debug flags
    uint8_t debugFlags;
} Engine;

extern Engine engine;

#endif