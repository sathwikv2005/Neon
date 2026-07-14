#ifndef neon_server_h
#define neon_server_h

#include "database.h"
#include "table.h"

typedef struct {
    Database database;
    Table strings;
} Server;

extern Server server;

#endif