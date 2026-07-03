#include "../include/memory.h"

#include "compiler.h"
#include "stdlib.h"
#include "vm.h"

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    vm.bytesAllocated += newSize - oldSize;

#ifdef NEON_DEBUG
    if (newSize > oldSize) {
        if (GET_DEBUG_STRESS_GC()) collectGarbage();
    }
#endif
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }
    void* newPointer = realloc(pointer, newSize);
    if (newPointer == NULL) exit(1);
    return newPointer;
}

/*
    Marks an object and pushes it onto the VM's gray stack so that its
   references can later be traversed and marked.
*/
void markObject(Obj* object) {
    if (object == NULL || object->isMarked == vm.currentGCMark) return;
#ifdef HELIUM_DEBUG
    if (GET_DEBUG_LOG_GC()) {
        printf("%p mark ", (void*)object);
        printValue(OBJ_VAL(object));
        printf("\n");
    }
#endif
    object->isMarked = vm.currentGCMark;

    if (vm.grayCapacity < vm.grayCount + 1) {
        vm.grayCapacity = GROW_CAPACITY(vm.grayCapacity);
        vm.grayStack = realloc(vm.grayStack, sizeof(Obj*) * vm.grayCapacity);
        if (vm.grayStack == NULL) exit(1);
    }

    vm.grayStack[vm.grayCount++] = object;
}

void markValue(Value value) {
    if (!IS_OBJ(value)) return;
    markObject(AS_OBJ(value));
}

static void markArray(ValueArray* array) {
    for (int i = 0; i < array->count; i++) {
        markValue(array->values[i]);
    }
}
/*
    Marks all objects referenced by this object.
*/
static void blackenObject(Obj* object) {
#ifdef HELIUM_DEBUG
    if (GET_DEBUG_LOG_GC()) {
        printf("%p blacken ", (void*)object);
        printValue(OBJ_VAL(object));
        printf("\n");
    }
#endif
    switch (object->type) {
        case OBJ_STRING:
            break;
    }
}

static void freeObject(Obj* object) {
#ifdef HELIUM_DEBUG
    if (GET_DEBUG_LOG_GC())
        printf("%p free type %d\n", (void*)object, object->type);
#endif
    switch (object->type) {
        case OBJ_STRING: {
            ObjString* string = (ObjString*)object;
            FREE_ARRAY(char, string->chars, string->length + 1);
            FREE(ObjString, object);
            break;
        }
    }
}

void freeObjects() {
    Obj* object = vm.objects;
    while (object != NULL) {
        Obj* next = object->next;
        freeObject(object);
        object = next;
    }

    free(vm.grayStack);
}

/*
    Mark all GC roots.

    Any object reachable from these roots is considered live.
*/
static void markRoots() {
    for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
        markValue(*slot);
    }
    markCompilerRoots();
}

/*
    Traverses the gray stack until all reachable objects are visited
*/
static void traceReferences() {
    while (vm.grayCount > 0) {
        Obj* object = vm.grayStack[--vm.grayCount];
        blackenObject(object);
    }
}

/*
    Frees every unmarked(unreachable) object from memory.
*/
static void sweep() {
    Obj* previous = NULL;
    Obj* object = vm.objects;
    while (object != NULL) {
        if (object->isMarked == vm.currentGCMark) {
            previous = object;
            object = object->next;
            continue;
        }
        Obj* unreached = object;
        object = object->next;
        if (previous != NULL) {
            previous->next = object;
        } else {
            vm.objects = object;
        }

        freeObject(unreached);
    }
}

/*
    Helium's mark and sweep garbage collector.

    collection happens in four phases:
      1. mark all roots.
      2. traverse and mark every reachable object.
      3. remove dead strings from the intern table.
      4. sweep and free all unmarked(unreachable) objects.

    Helium uses a toggle mark bit (`currentGCMark`) instead of clearing every
    object's mark before every collection cycle. After a sweep, the global mark
    value is flipped, effectively making all remaining objects as unmarked for
    the next GC cycle.
 */
void collectGarbage() {
#ifdef HELIUM_DEBUG
    size_t before = 0;
    if (GET_DEBUG_LOG_GC()) {
        printf("------ gc begin\n");
        before = vm.bytesAllocated;
    }
#endif

    markRoots();
    traceReferences();
    // tableRemoveWhite(&vm.strings);
    sweep();

    // flip the mark bit, so all surviving objects appear as unmarked for next
    // cycle.
    vm.currentGCMark = !vm.currentGCMark;

    vm.nextGC = vm.bytesAllocated * GC_HEAP_GROW_FACTOR;

#ifdef HELIUM_DEBUG
    if (GET_DEBUG_LOG_GC()) {
        printf("------ gc end\n");
        printf("   collected %zu bytes (from %zu to %zu) next at %zu\n",
               before - vm.bytesAllocated, before, vm.bytesAllocated,
               vm.nextGC);
    }
#endif
}