#include "database.h"

#include "../include/memory.h"
#include "file.h"
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

// load database from save/checkpoint onto memory
Database* loadDatabase(uint8_t id) {
    if (id >= MAX_DATABASE) return NULL;

    if (server.database[id].loaded) return &server.database[id];
    initDatabase(&server.database[id]);

    // TODO: load database from save/checkpoint onto memory

    server.database[id].id = id;
    return &server.database[id];
}

// if no active clients, save database and free the database from memory
bool unloadDatabase(Database* database) {
    if (database->clients > 0) return;

    if (!saveDatabase(database)) {
        return false;
    }

    freeDatabase(database);

    return true;
}

// write a complete snapshot of the database to disk.
bool saveDatabase(Database* database) {
    // TODO: write a complete snapshot of the database to disk.

    return false;  // unimplemented
}

// sync pending in memory changes with the database file on disk
bool syncDatabase(Database* database) {
    // TODO: sync pending in memory changes with the database file on disk

    return false;  // unimplemented
}