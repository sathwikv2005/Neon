#include "engine.h"

#include "../include/memory.h"

Engine* createEngine() {
    Engine* engine = malloc(sizeof(Engine));
    if (engine == NULL) return NULL;

    initvm(&engine->vm);
    return engine;
}

void freeEngine(Engine* engine) {
    freevm(&engine->vm);
    free(engine);
}