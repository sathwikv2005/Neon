#ifndef neon_engine_common_h
#define neon_engine_common_h

#include "../../include/memory.h"
#include "engine.h"
#include "object.h"

// util
Value peek(int distance);
bool isFalsey(Value value);
void concatenate();
void resetStack();

#endif