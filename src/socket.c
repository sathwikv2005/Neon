#include "socket.h"

bool initSockets() {
#ifdef _WIN32
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
#endif
    return true;
}

void freeSockets() {
#ifdef _WIN32
    WSACleanup();
#endif
}