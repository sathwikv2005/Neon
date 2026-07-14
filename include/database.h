#ifndef neon_database_h
#define neon_database_h

#include "table.h"

typedef struct Database {
    Table table;
    Obj* objects;
    size_t bytesAllocated;
    size_t clients;

} Database;

#endif