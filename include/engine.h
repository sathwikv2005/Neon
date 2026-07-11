#ifndef neon_engine_h
#define neon_engine_h

#include "server.h"
#include "table.h"
#include "vm.h"

typedef struct {
    VM vm;
} Engine;

Engine* createEngine();
void freeEngine(Engine* engine);

#endif