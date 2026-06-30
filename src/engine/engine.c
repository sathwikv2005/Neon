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
    // initTable(&engine.strings);
}

void freeEngine() {
    freeObjects();
    // freeTable(&engine.strings);
}
