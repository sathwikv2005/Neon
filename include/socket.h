#ifndef neon_socket_h
#define neon_socket_h

#ifdef _WIN32

#include <winsock2.h>
typedef SOCKET Socket;
#define INVALID_SOCKET_VALUE INVALID_SOCKET

#else

#include <sys/socket.h>
typedef int Socket;
#define INVALID_SOCKET_VALUE (-1)

#endif

#endif