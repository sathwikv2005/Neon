#ifndef helium_line_h
#define helium_line_h

#include "common.h"

typedef struct {
    int line;
    int count;
} LineRun;

typedef struct {
    int count;
    int capacity;
    LineRun* runs;
} LineArray;

void initLineArray(LineArray* lines);
void freeLineArray(LineArray* lines);

#endif