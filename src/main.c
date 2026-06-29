#include <stdio.h>

#include "common.h"
#include "engine.h"

#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif

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
    for (;;) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        // run command.
    }
}

int main(int argc, const char* argv[]) {
    repl();
    return 0;
}