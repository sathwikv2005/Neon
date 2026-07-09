#include "engine.h"

#include "../include/memory.h"

Engine* createEngine() {
    Engine* engine = ALLOCATE(Engine, 1);
    initvm(&engine->vm);
    return engine;
}

void freeEngine(Engine* engine) {
    freevm(&engine->vm);
    FREE(Engine, engine);
}