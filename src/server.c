#include "server.h"

Server server;

void initServer() {
    initTable(&server.database);
    initTable(&server.strings);
}

void freeServer(void) {
    freeTable(&server.database);
    freeTable(&server.strings);
}