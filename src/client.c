#include "client.h"

#include <stdio.h>

#include "engine.h"
#include "logger.h"
#include "vm.h"

void initClient(Client* client, ClientType type) { client->type = type; }

void freeClient(Client* client) {
    (void)client;  // to remove GCC's unused parameter warning.

    // nothing to free yet
}

static bool respondRESP(Engine* engine, InterpretOutput result) {
    // unimplemented
    return false;
}

static bool respondRepl(Engine* engine, InterpretOutput result) {
    switch (result.status) {
        case INTERPRET_COMPILE_ERROR:
            fprintf(stderr,
                    ANSI_BOLD ANSI_RED "Syntax error: " ANSI_RESET "%s\n",
                    engine->vm.error);
            break;

        case INTERPRET_RUNTIME_ERROR:
            fprintf(stderr,
                    ANSI_BOLD ANSI_RED "Command error: " ANSI_RESET "%s\n",
                    engine->vm.error);
            break;

        case INTERPRET_EXIT:
            return false;

        case INTERPRET_OK:
            if (!result.hasValue) {
                printf("OK\n");
                break;
            }
            printValue(result.value);
            printf("\n");
            break;
    }
    return true;
}

bool respond(Engine* engine, InterpretOutput result) {
    switch (engine->client.type) {
        case CLIENT_REPL:
            return respondRepl(engine, result);

        case CLIENT_RESP:
            return respondRESP(engine, result);
    }
    LOG_FATAL("Invalid client type: %d", (int)engine->client.type);
    return false;
}