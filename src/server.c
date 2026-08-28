#include "server.h"

#include "logger.h"
#include "socket.h"

Server server;

bool initServer() {
    initTable(&server.strings);
    if (!initSockets()) {
        LOG_FATAL("Failed to initialize sockets.");
        return false;
    }
    return true;
}

void freeServer() {
    freeTable(&server.strings);
    freeSockets();
}