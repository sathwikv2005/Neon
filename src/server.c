#include "server.h"

Server server;

void initServer() {
    initTable(&server.database.table);
    initTable(&server.strings);
}

void freeServer(void) {
    freeTable(&server.database.table);
    freeTable(&server.strings);
}