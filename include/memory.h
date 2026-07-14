#ifndef NEON_MEMORY_H
#define NEON_MEMORY_H

#include <stdlib.h>

#include "common.h"
#include "database.h"
#include "object.h"

#define GC_HEAP_GROW_FACTOR 2

// Normal allocations (not tracked by the VM)
#define ALLOCATE(type, count) ((type*)malloc(sizeof(type) * (count)))

#define FREE(type, pointer) free(pointer)

#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    ((type*)realloc(pointer, sizeof(type) * (newCount)))

#define FREE_ARRAY(type, pointer) free(pointer)

// Database managed allocations
#define DATABASE_ALLOCATE(database, type, count) \
    ((type*)reallocate(database, NULL, 0, sizeof(type) * (count)))

#define DATABASE_FREE(database, type, pointer) \
    reallocate(database, pointer, sizeof(type), 0)

#define DATABASE_GROW_ARRAY(database, type, pointer, oldCount, newCount) \
    ((type*)reallocate(database, pointer, sizeof(type) * (oldCount),     \
                       sizeof(type) * (newCount)))

#define DATABASE_FREE_ARRAY(database, type, pointer, oldCount) \
    reallocate(database, pointer, sizeof(type) * (oldCount), 0)

void* reallocate(Database* database, void* pointer, size_t oldSize,
                 size_t newSize);
void freeObjects(Database* database);

#endif