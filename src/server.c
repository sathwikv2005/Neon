#include "server.h"

Server server;

void initServer() {
    server.bytesAllocated = 0;
    initTable(&server.database);
    initTable(&server.strings);
    server.objects = NULL;
}