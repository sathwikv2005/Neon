#ifndef neon_socket_h
#define neon_socket_h

#include "common.h"

#define SOCKET_BACKLOG 128

#ifdef _WIN32

#include <winsock2.h>
typedef SOCKET Socket;
#define INVALID_SOCKET_VALUE INVALID_SOCKET

#else

#include <sys/socket.h>
typedef int Socket;
#define INVALID_SOCKET_VALUE (-1)

#endif

bool initSockets();
void freeSockets();

Socket createSocket();
bool bindSocket(Socket socket, uint16_t port);
bool listenSocket(Socket socket);
Socket acceptSocket(Socket socket);

int recvSocket(Socket socket, char* buffer, int length);
int sendSocket(Socket socket, const char* buffer, int length);

void closeSocket(Socket socket);

#endif