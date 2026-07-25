#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "engine.h"
#include "logger.h"
#include "vm.h"

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
    signal(SIGINT, SIG_IGN);  // ignore further Ctrl+C presses.
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

static void repl() {
    if (isatty(fileno(stdin))) {
        printBanner();
    }
    char line[1024];

    // TODO: take database id as input from user and pass here. Default = 0
    uint8_t id = 0;
    Engine* engine = createEngine(CLIENT_REPL, id);
    if (engine == NULL) {
        printf(
            "Failed to start a repl session please check the log file for "
            "errors.");
        return;
    }
    for (;;) {
        if (shouldExit) break;
        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        InterpretOutput result = execute(engine, line);
        if (result.status == INTERPRET_EXIT) break;
        if (!respond(engine, result)) break;
    }
    freeEngine(engine);
}

int main(int argc, const char* argv[]) {
    signal(SIGINT, handleSigInt);
    if (!initLogger(LOG_FILE_PATH)) {
        printf("failed to initilize the logger\n");
        return 1;
    }
    repl();
    closeLogger();
    return 0;
}