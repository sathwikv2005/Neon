#include "object.h"

#include <stdio.h>
#include <string.h>

#include "../include/memory.h"
#include "server.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
    Obj* object = (Obj*)reallocate(NULL, 0, size);
    object->type = type;
    object->survived = false;
    // object->isMarked = !vm.currentGCMark;

    // object->next = database->objects;
    // database->objects = object;

    // #ifdef HELIUM_DEBUG
    //     if (GET_DEBUG_LOG_GC())
    //         printf("%p allocate %zu for %d\n", (void*)object, size, type);
    // #endif

    return object;
}

static ObjString* allocateString(VM* vm, char* chars, int length,
                                 uint32_t hash) {
    ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;
    string->ref = 0;

    // push(vm, OBJ_VAL(string));

    tableSet(&server.strings, string, NULL_VAL);

    // pop(vm);

    return string;
}

static uint32_t hashString(const char* key, int length) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

/*
    Every unique string is interned exactly once.

    This reduces duplicate allocations and allows string equality to be just a
    simple pointer comparison.
 */
ObjString* takeString(VM* vm, char* chars, int length) {
    uint32_t hash = hashString(chars, length);

    ObjString* interned = tableFindString(&server.strings, chars, length, hash);

    if (interned != NULL) {
        FREE_ARRAY(char, chars, length + 1);
        return interned;
    }

    return allocateString(vm, chars, length, hash);
}

/*
    Every unique string is interned exactly once.

    This reduces duplicate allocations and allows string equality to be just a
    simple pointer comparison.
 */
ObjString* copyString(VM* vm, const char* chars, int length) {
    uint32_t hash = hashString(chars, length);

    ObjString* interned = tableFindString(&server.strings, chars, length, hash);
    if (interned != NULL) {
        return interned;
    }

    char* heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return allocateString(vm, heapChars, length, hash);
}

static Value stringValue(VM* vm, const char* chars) {
    return OBJ_VAL(copyString(vm, chars, (int)strlen(chars)));
}

static void printString(char* str, bool withQuotes) {
    if (withQuotes) {
        printf("\"%s\"", str);
    } else {
        printf("%s", str);
    }
    return;
}

void printObject(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printString(AS_CSTRING(value), false);
            break;
        default:
            break;
    }
}

ObjString* objTypeName(VM* vm, Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            return copyString(vm, "string", 6);
    }

    return copyString(vm, "unknown type", 12);
}

Value valueToString(VM* vm, Value value) {
    char buffer[32];

    if (IS_NULL(value)) {
        return stringValue(vm, "null");
    }

    if (IS_NUMBER(value)) {
        int length =
            snprintf(buffer, sizeof(buffer), "%.15g", AS_NUMBER(value));
        return OBJ_VAL(copyString(vm, buffer, length));
    }

    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            return value;
    }

    return NULL_VAL;
}