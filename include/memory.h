#ifndef NEON_MEMORY_H
#define NEON_MEMORY_H

#include <stdlib.h>

#include "common.h"
#include "object.h"

#define GC_HEAP_GROW_FACTOR 2

// Normal allocations (not tracked by the VM)
#define ALLOCATE(type, count) ((type*)malloc(sizeof(type) * (count)))

#define FREE(type, pointer) free(pointer)

#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    ((type*)realloc(pointer, sizeof(type) * (newCount)))

#define FREE_ARRAY(type, pointer) free(pointer)

// VM managed allocations
#define VM_ALLOCATE(vm, type, count) \
    ((type*)reallocate(vm, NULL, 0, sizeof(type) * (count)))

#define VM_FREE(vm, type, pointer) reallocate(vm, pointer, sizeof(type), 0)

#define VM_GROW_ARRAY(vm, type, pointer, oldCount, newCount)   \
    ((type*)reallocate(vm, pointer, sizeof(type) * (oldCount), \
                       sizeof(type) * (newCount)))

#define VM_FREE_ARRAY(vm, type, pointer, oldCount) \
    reallocate(vm, pointer, sizeof(type) * (oldCount), 0)

void* reallocate(VM* vm, void* pointer, size_t oldSize, size_t newSize);
void freeObjects(VM* vm);

#endif