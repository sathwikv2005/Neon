#include "engine.h"

#include "../include/memory.h"
#include "logger.h"

Engine* createEngine(ClientType type) {
    Engine* engine = malloc(sizeof(Engine));
    if (engine == NULL) return NULL;

    initvm(&engine->vm);
    initClient(&engine->client, type);

    // TODO: take database id as input from user and pass here. Default = 0
    uint8_t id = 0;
    Database* database = loadDatabase(id);
    if (database == NULL) {
        LOG_ERROR("Unable to load database(%u)", (unsigned)id);
        return NULL;
    }

    engine->vm.database = database;
    database->clients += 1;
    return engine;
}

void freeEngine(Engine* engine) {
    Database* db = engine->vm.database;
    db->clients--;
    if (!unloadDatabase(db)) {
        LOG_FATAL("Saving database(%u) failed on connection close.",
                  (unsigned)db->id);
    }
    freevm(&engine->vm);
    freeClient(&engine->client);
    free(engine);
}