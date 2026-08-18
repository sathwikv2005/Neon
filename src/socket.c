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

Socket createSocket() {
    Socket socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFd == INVALID_SOCKET_VALUE) {
        return INVALID_SOCKET_VALUE;
    }

    return socketFd;
}