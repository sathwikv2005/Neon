#ifndef neon_vm_common_h
#define neon_vm_common_h

#include "../../include/memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

// util
Value peek(VM* vm, int distance);
bool isFalsey(Value value);
void concatenate(VM* vm);
void resetStack(VM* vm);

#endif