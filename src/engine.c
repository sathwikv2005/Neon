#include "engine.h"

#include "../include/memory.h"

Engine* createEngine() {
    Engine* engine = malloc(sizeof(Engine));
    if (engine == NULL) return NULL;

    initvm(&engine->vm);
    Database* database = loadDatabase(0);
    engine->vm.database = database;
    database->clients += 1;
    return engine;
}

void freeEngine(Engine* engine) {
    Database* db = engine->vm.database;
    db->clients--;
    freevm(&engine->vm);
    free(engine);
}