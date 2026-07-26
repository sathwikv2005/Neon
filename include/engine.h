#ifndef neon_engine_h
#define neon_engine_h

#include "client.h"
#include "server.h"
#include "table.h"
#include "vm.h"

typedef struct Engine {
    VM vm;
    Client client;
} Engine;

Engine* createEngine(ClientType type, uint8_t id);
void freeEngine(Engine* engine);

InterpretOutput execute(Engine* engine, const char* line);

#endif