#include "engine.h"

#include "../include/memory.h"
#include "logger.h"

Engine* createEngine() {
    Engine* engine = malloc(sizeof(Engine));
    if (engine == NULL) return NULL;

    initvm(&engine->vm);

    // TODO: take database id as input from user and pass here. Default = 0
    Database* database = loadDatabase(0);

    engine->vm.database = database;
    database->clients += 1;
    return engine;
}

void freeEngine(Engine* engine) {
    Database* db = engine->vm.database;
    db->clients--;
    if (!unloadDatabase(db)) {
        LOG_FATAL("Saving database(%d) failed.", db->id);
    }
    freevm(&engine->vm);
    free(engine);
}