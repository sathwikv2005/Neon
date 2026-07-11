#include "line.h"

// use a relative path to avoid conflicting with the system <memory.h>
#include "../include/memory.h"
#include "common.h"

void initLineArray(LineArray* lines) {
    lines->count = 0;
    lines->capacity = 0;
    lines->runs = NULL;
}

void freeLineArray(LineArray* lines) {
    FREE_ARRAY(LineRun, lines->runs);
    initLineArray(lines);
}