#include <stdio.h>

#include "common.h"
#include "engine.h"

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

int main(int argc, const char* argv[]) {
    printBanner();
    repl();
    return 0;
}