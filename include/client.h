#ifndef neon_client_h
#define neon_client_h

#include "common.h"
#include "socket.h"

typedef struct Engine Engine;
typedef struct InterpretOutput InterpretOutput;

typedef enum {
    CLIENT_REPL,
    CLIENT_RESP,
} ClientType;

typedef struct Client {
    ClientType type;
    Socket socket;
} Client;

void initClient(Client* client, ClientType type);
void freeClient(Client* client);

bool respond(Engine* engine, InterpretOutput result);

#endif