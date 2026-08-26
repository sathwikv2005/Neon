#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "engine.h"
#include "logger.h"
#include "server.h"

#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif

#include <signal.h>

volatile sig_atomic_t shouldExit = 0;

static void handleSigInt(int sig) {
    (void)sig;
    shouldExit = 1;
}

static void printBanner() {
    printf(ANSI_NEON_BLUE);

    printf("############################################################\n");
    printf("#                                                          #\n");
    printf("#      _   _                    _                          #\n");
    printf("#     | \\ | |                  | |                         #\n");
    printf("#     |  \\| | ___  ___  _ __   | |                         #\n");
    printf("#     | . ` |/ _ \\/ _ \\| '_ \\  | |                         #\n");
    printf("#     | |\\  |  __/ (_) | | | | |_|                         #\n");
    printf("#     |_| \\_|\\___|\\___/|_| |_| (_)                         #\n");
    printf("#                                                          #\n");
    printf(ANSI_ITALIC);
    printf("#                       Neon v%-29s#\n", NEON_VERSION_STRING);
    printf("#              Bright. Fast. Built to Last.                #\n");
    printf("#                                                          #\n");
    printf("############################################################\n\n");

    printf(ANSI_RESET);
}

static void repl(uint8_t databaseId) {
    if (isatty(fileno(stdin))) {
        printBanner();
    }
    char line[1024];

    Engine* engine = createEngine(CLIENT_REPL, 0, databaseId);
    if (engine == NULL) {
        printf(
            "Failed to start a repl session. Please check the log file for "
            "errors.\n");
        return;
    }
    for (;;) {
        if (shouldExit) {
            break;
        }

        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        InterpretOutput result = execute(engine, line);

        if (result.status == INTERPRET_EXIT) {
            break;
        }

        if (!respond(engine, result)) {
            LOG_ERROR("Failed to send response to %s client.",
                      engine->client.type == CLIENT_REPL ? "REPL" : "RESP");
            break;
        }
    }

    freeEngine(engine);
}

static bool respClientSession(Socket client) {
    Engine* engine = createEngine(CLIENT_RESP, client, 0);

    if (engine == NULL) {
        LOG_ERROR("Failed to create client engine.");
        return false;
    }

    bool success = true;

    while (!shouldExit) {
        char buffer[CLIENT_BUFFER_SIZE + 1];

        int received = recvSocket(client, buffer, sizeof(buffer));

        if (received <= 0) {
            if (received < 0) {
                LOG_ERROR("Failed to receive client request.");
                success = false;
            }

            break;
        }

        buffer[received] = '\0';

        InterpretOutput output = execute(engine, buffer);

        if (!respond(engine, output)) {
            LOG_ERROR("Failed to send response to client.");
            success = false;
            break;
        }
    }

    freeEngine(engine);
    return success;
}

static bool serverSession(void) {
    const uint16_t port = 6379;

    LOG_INFO("Creating server socket.");

    Socket server = createSocket();

    if (server == INVALID_SOCKET_VALUE) {
        LOG_ERROR("Failed to create server socket.");
        return false;
    }

    LOG_DEBUG("Server socket created.");

    if (!bindSocket(server, port)) {
        LOG_ERROR("Failed to bind server socket to port %u.", port);
        closeSocket(server);
        return false;
    }

    LOG_INFO("Server socket bound to port %u.", port);

    if (!listenSocket(server)) {
        LOG_ERROR("Failed to listen on server socket.");
        closeSocket(server);
        return false;
    }

    LOG_INFO("Server listening on port %u.", port);

    printBanner();

    while (!shouldExit) {
        LOG_DEBUG("Waiting for client connection.");

        Socket client = acceptSocket(server);

        if (client == INVALID_SOCKET_VALUE) {
            if (shouldExit) {
                break;
            }

            LOG_ERROR("Failed to accept client connection.");
            continue;
        }

        LOG_INFO("Client connection accepted.");

        if (!respClientSession(client)) {
            LOG_DEBUG("Client session ended with an error.");
        }

        closeSocket(client);

        LOG_DEBUG("Client connection closed.");
    }

    LOG_INFO("Stopping server.");

    closeSocket(server);

    return true;
}

static void usage(const char* program) {
    printf("Usage:\n");
    printf("  %s                 Start server\n", program);
    printf("  %s --server        Start server\n", program);
    printf("  %s --repl          Start REPL\n", program);
    printf("  %s --repl --db N   Start REPL using database N\n", program);
    printf("  %s --repl -d N     Same as above\n", program);
}

int main(int argc, const char* argv[]) {
    signal(SIGINT, handleSigInt);

    if (!initLogger(LOG_FILE_PATH)) {
        printf("Failed to initialize the logger.\n");
        return 1;
    }

    // TODO: log and exit if initServer() fails.
    initServer();

    bool startRepl = false;
    uint8_t databaseId = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--repl") == 0) {
            startRepl = true;
        } else if (strcmp(argv[i], "--server") == 0) {
            startRepl = false;
        } else if (strcmp(argv[i], "--db") == 0 || strcmp(argv[i], "-d") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "Missing database id.\n");
                usage("neon");
                goto cleanup;
            }

            char* end;
            long value = strtol(argv[i], &end, 10);

            if (*end != '\0' || value < 0 || value >= MAX_DATABASE) {
                fprintf(stderr, "Database id must be between 0 and %d.\n",
                        MAX_DATABASE - 1);
                goto cleanup;
            }

            databaseId = (uint8_t)value;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            usage("neon");
            goto cleanup;
        }
    }

    if (startRepl) {
        repl(databaseId);
    } else if (!serverSession()) {
        LOG_ERROR("Server session failed.");
        fprintf(stderr, "Server session failed.\n");
    }

cleanup:
    closeLogger();
    freeServer();

    return 0;
}