#include "object.h"

#include <stdio.h>
#include <string.h>

#include "../include/memory.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
    Obj* object = (Obj*)reallocate(NULL, 0, size);
    object->type = type;
    object->isMarked = !vm.currentGCMark;
    object->next = vm.objects;
    vm.objects = object;

#ifdef HELIUM_DEBUG
    if (GET_DEBUG_LOG_GC())
        printf("%p allocate %zu for %d\n", (void*)object, size, type);
#endif

    return object;
}