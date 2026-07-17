#ifndef neon_database_h
#define neon_database_h

#include "table.h"

typedef struct Database {
    // Database state
    bool loaded;
    uint8_t id;
    size_t clients;

    Table table;

} Database;

Database* loadDatabase(uint8_t id);
bool unloadDatabase(Database* db);
bool saveDatabase(Database* db);
bool syncDatabase(Database* db);

#endif