#ifndef neon_vm_common_h
#define neon_vm_common_h

#include "../../include/memory.h"
#include "object.h"
#include "vm.h"

// util
Value peek(int distance);
bool isFalsey(Value value);
void concatenate();
void resetStack();

#endif