#include "database.h"

#include "../include/memory.h"
#include "file.h"
#include "logger.h"
#include "serializer.h"
#include "server.h"

static void initDatabase(Database* database) {
    initTable(&database->table);
    database->clients = 0;
    database->loaded = true;
}

static void freeDatabase(Database* database) {
    freeTable(&database->table);
    database->clients = 0;
    database->loaded = false;
}

static void getSnapShotPath(char* path, size_t size, uint8_t id) {
    snprintf(path, size, "./db/%u/snapshot.ne", (unsigned)id);
}

// load database from save/checkpoint onto memory
Database* loadDatabase(uint8_t id) {
    if (id >= MAX_DATABASE) {
        LOG_ERROR("Invalid database id: %u", (unsigned)id);
        return NULL;
    }

    if (server.database[id].loaded) {
        LOG_DEBUG("Database(%u) already loaded", (unsigned)id);
        return &server.database[id];
    }

    initDatabase(&server.database[id]);
    server.database[id].id = id;

    if (!readDatabase(&server.database[id])) {
        LOG_WARN(
            "Failed to load snapshot for database(%u), starting with empty "
            "database",
            (unsigned)id);
    } else {
        LOG_INFO("Loaded database(%u)", (unsigned)id);
    }

    return &server.database[id];
}

// save database and if no active clients, free the database from memory
bool unloadDatabase(Database* database) {
    if (!saveDatabase(database)) {
        LOG_ERROR("Saving database(%u) failed", (unsigned)database->id);
        return false;
    }

    if (database->clients > 0) {
        LOG_DEBUG("Database(%u) still has %zu active client(s)",
                  (unsigned)database->id, database->clients);
        return false;
    }

    freeDatabase(database);
    LOG_INFO("Unloaded database(%u)", (unsigned)database->id);

    return true;
}

// write a complete snapshot of the database to disk.
bool saveDatabase(Database* database) {
    char path[64];
    getSnapShotPath(path, sizeof(path), database->id);

    File file;
    if (!fileOpen(&file, path, FILE_WRITE)) {
        LOG_ERROR("Failed to open \"%s\" for writing", path);
        return false;
    }

    if (!writeTable(&file, &database->table)) {
        LOG_ERROR("Failed to write snapshot \"%s\"", path);
        fileClose(&file);
        return false;
    }

    fileClose(&file);
    return true;
}

// sync pending in memory changes with the database file on disk
bool syncDatabase(Database* database) {
    // TODO: sync pending in memory changes with the database file on disk

    return false;  // unimplemented
}

// read database from snapshot file
bool readDatabase(Database* database) {
    char path[64];
    getSnapShotPath(path, sizeof(path), database->id);

    File file;
    if (!fileOpen(&file, path, FILE_READ)) {
        LOG_WARN("Snapshot \"%s\" does not exist or cannot be opened", path);
        return false;
    }

    if (!readTable(&file, &database->table)) {
        LOG_ERROR("Failed to read snapshot \"%s\"", path);
        fileClose(&file);
        return false;
    }

    fileClose(&file);
    return true;
}