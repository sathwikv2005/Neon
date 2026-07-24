#ifndef neon_client_h
#define neon_client_h

typedef enum {
    CLIENT_REPL,
    CLIENT_RESP,
} ClientType;

typedef struct Client {
    ClientType type;
} Client;

void initClient(Client* client, ClientType type);

#endif