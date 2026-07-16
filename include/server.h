#ifndef neon_server_h
#define neon_server_h

#include "database.h"
#include "table.h"

#define MAX_DATABASE 8

typedef struct {
    Database database[MAX_DATABASE];
    Table strings;
} Server;

extern Server server;

#endif