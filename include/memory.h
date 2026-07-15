#ifndef NEON_MEMORY_H
#define NEON_MEMORY_H

#include <stdlib.h>

#include "common.h"
#include "database.h"
#include "object.h"

#define GC_HEAP_GROW_FACTOR 2

#define ALLOCATE(type, count) (type*)reallocate(NULL, 0, sizeof(type) * (count))

#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0)

// grows capacity: minimum 8, otherwise doubles.
#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)

// grows the array from oldCount to newCount
#define GROW_ARRAY(type, pointer, oldCount, newCount)     \
    (type*)reallocate(pointer, sizeof(type) * (oldCount), \
                      sizeof(type) * (newCount))

// deallocates the memory assigned to the array
#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

void* reallocate(void* pointer, size_t oldSize, size_t newSize);
void freeObject(Obj* object);
void retainString(ObjString* string);
void releaseString(ObjString* string);
void releaseObject(Obj* obj);
void retainObject(Obj* obj);
// void freeObjects(Database* database);

#endif