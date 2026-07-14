#include "database.h"

#include "../include/memory.h"
#include "server.h"

static void initDatabase(Database* database) {
    initTable(&database->table);
    database->clients = 0;
    database->bytesAllocated = 0;
    database->objects = NULL;
}

static void freeDatabase(Database* database) {
    freeTable(&database->table);
    freeObjects(database);
}

// returns the total bytes allocated for the database objects
size_t getBytesAllocated(Database* database) {
    return database->bytesAllocated;
}

// load database from save/checkpoint onto memory
Database* loadDatabase(uint8_t id) {
    // TODO: load database from save/checkpoint onto memory
    // TODO: implement multiple databases

    // TODO: this reinitializes db for every client that loads the same db.
    // TODO: need to keep trak on initialized dbs in Server.
    // if(server[id]) return server.databases[id];
    // server[id] = true;
    initDatabase(&server.database);
    return &server.database;
}

// if no active clients, save database and free the database from memory
void unloadDatabase(Database* database) {
    if (database->clients > 0) return;
    // TODO: if no active clients, save the database onto a file
    freeDatabase(database);
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