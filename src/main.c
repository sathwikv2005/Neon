#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "engine.h"
#include "vm.h"

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
    Engine* engine = createEngine();
    for (;;) {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        InterpretOutput result = interpret(line, &engine->vm);

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
                freeEngine(engine);
                return;

            case INTERPRET_OK:
                if (!result.hasValue) {
                    printf("OK\n");
                    break;
                }
                printValue(result.value);
                printf("\n");
                break;
        }
    }
    freeEngine(engine);
}

int main(int argc, const char* argv[]) {
    repl();
    return 0;
}