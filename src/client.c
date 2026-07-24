#include "client.h"

void initClient(Client* client, ClientType type) { client->type = type; }

void freeClient(Client* client) {
    (void)client;  // to remove GCC's unused parameter warning.

    // nothing to free yet
}