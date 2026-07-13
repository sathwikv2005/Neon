#ifndef neon_server_h
#define neon_server_h

#include "table.h"
typedef struct Database {
    Table table;
    Obj* objects;
    size_t bytesAllocated;
    size_t clients;

} Database;

typedef struct {
    Database database;
    Table strings;
} Server;

extern Server server;

#endif