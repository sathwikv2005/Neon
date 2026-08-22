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
    // IPv4 TCP connection
    Socket socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFd == INVALID_SOCKET_VALUE) {
        return INVALID_SOCKET_VALUE;
    }

    return socketFd;
}

bool bindSocket(Socket socketFd, uint16_t port) {
    struct sockaddr_in address = {
        .sin_family = AF_INET,  // IPv4 address type
        .sin_addr.s_addr = htonl(
            INADDR_ANY),  // listen on all available local network interfaces.
        .sin_port = htons(port),
    };

    return bind(socketFd, (struct sockaddr*)&address, sizeof(address)) == 0;
}

bool listenSocket(Socket socketFd) {
    return listen(socketFd, SOCKET_BACKLOG) == 0;
}

Socket acceptSocket(Socket socketFd) { return accept(socketFd, NULL, NULL); }

void closeSocket(Socket socketFd) {
#ifdef _WIN32
    closesocket(socketFd);
#else
    close(socketFd);
#endif
}

int recvSocket(Socket socketFd, char* buffer, int length) {
    return recv(socketFd, buffer, length, 0);
}

int sendSocket(Socket socketFd, const char* buffer, int length) {
    return send(socketFd, buffer, length, 0);
}