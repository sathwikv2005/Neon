#ifndef neon_server_h
#define neon_server_h

#include "table.h"

typedef struct {
    Table database;
    Table strings;

    // gc
    size_t bytesAllocated;
    Obj* objects;
    // size_t nextGC;
    // int grayCount;
    // int grayCapacity;
    // Obj** grayStack;
    // bool currentGCMark;

} Server;

extern Server server;

#endif