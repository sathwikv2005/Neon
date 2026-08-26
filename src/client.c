#include "client.h"

#include <stdio.h>

#include "engine.h"
#include "logger.h"
#include "vm.h"

void initClient(Client* client, ClientType type, Socket socket) {
    client->type = type;
    client->socket = socket;
}

void freeClient(Client* client) {
    (void)client;  // to remove GCC's unused parameter warning.

    // nothing to free yet
}

static bool sendRESP(Engine* engine, const char* data, size_t length) {
    return sendSocket(engine->client.socket, data, length);
}

static bool respondRESP(Engine* engine, InterpretOutput result) {
    char buffer[256];

    switch (result.status) {
        case INTERPRET_COMPILE_ERROR:
        case INTERPRET_RUNTIME_ERROR: {
            int length =
                snprintf(buffer, sizeof(buffer), "-%s\r\n", engine->vm.error);

            if (length < 0 || (size_t)length >= sizeof(buffer)) {
                return false;
            }

            return sendRESP(engine, buffer, (size_t)length);
        }

        case INTERPRET_EXIT:
            return false;

        case INTERPRET_OK:
            break;
    }

    if (!result.hasValue) {
        static const char response[] = "+OK\r\n";
        return sendRESP(engine, response, sizeof(response) - 1);
    }

    Value value = result.value;

    if (IS_NULL(value)) {
        static const char response[] = "$-1\r\n";
        return sendRESP(engine, response, sizeof(response) - 1);
    }

    if (IS_NUMBER(value)) {
        int length =
            snprintf(buffer, sizeof(buffer), ":%.0f\r\n", AS_NUMBER(value));

        if (length < 0 || (size_t)length >= sizeof(buffer)) {
            return false;
        }

        return sendRESP(engine, buffer, (size_t)length);
    }

    if (IS_STRING(value)) {
        ObjString* string = AS_STRING(value);

        int headerLength =
            snprintf(buffer, sizeof(buffer), "$%d\r\n", string->length);

        if (headerLength < 0 || (size_t)headerLength >= sizeof(buffer)) {
            return false;
        }

        if (!sendRESP(engine, buffer, (size_t)headerLength)) {
            return false;
        }

        if (!sendRESP(engine, string->chars, string->length)) {
            return false;
        }

        static const char crlf[] = "\r\n";
        return sendRESP(engine, crlf, 2);
    }

    Value stringValue = valueToString(value);

    if (IS_STRING(stringValue)) {
        ObjString* string = AS_STRING(stringValue);

        int headerLength =
            snprintf(buffer, sizeof(buffer), "$%d\r\n", string->length);

        if (headerLength < 0 || (size_t)headerLength >= sizeof(buffer)) {
            return false;
        }

        if (!sendRESP(engine, buffer, (size_t)headerLength)) {
            return false;
        }

        if (!sendRESP(engine, string->chars, string->length)) {
            return false;
        }

        static const char crlf[] = "\r\n";
        return sendRESP(engine, crlf, 2);
    }

    LOG_ERROR("Unsupported value type in RESP response.");

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