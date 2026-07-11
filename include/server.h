#ifndef neon_server_h
#define neon_server_h

#include "table.h"

typedef struct {
    Table database;
    Table strings;
} Server;

extern Server server;

#endif