#include "server.h"

#include "logger.h"
#include "socket.h"

Server server;

void initServer() {
    initTable(&server.strings);
    if (!initSockets()) {
        LOG_FATAL("Failed to initialize sockets.");
        return;
    }
}

void freeServer() {
    freeTable(&server.strings);
    freeSockets();
}