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
    // object->isMarked = !vm.currentGCMark;
    object->next = vm.objects;
    vm.objects = object;

    // #ifdef HELIUM_DEBUG
    //     if (GET_DEBUG_LOG_GC())
    //         printf("%p allocate %zu for %d\n", (void*)object, size, type);
    // #endif

    return object;
}

static ObjString* allocateString(char* chars, int length, uint32_t hash) {
    ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;

    push(OBJ_VAL(string));

    tableSet(&vm.strings, string, NULL_VAL);

    pop();

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
ObjString* takeString(char* chars, int length) {
    uint32_t hash = hashString(chars, length);

    ObjString* interned = tableFindString(&vm.strings, chars, length, hash);

    if (interned != NULL) {
        FREE_ARRAY(char, chars, length + 1);
        return interned;
    }

    return allocateString(chars, length, hash);
}

/*
    Every unique string is interned exactly once.

    This reduces duplicate allocations and allows string equality to be just a
    simple pointer comparison.
 */
ObjString* copyString(const char* chars, int length) {
    uint32_t hash = hashString(chars, length);

    ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
    if (interned != NULL) return interned;

    char* heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return allocateString(heapChars, length, hash);
}

static Value stringValue(const char* chars) {
    return OBJ_VAL(copyString(chars, (int)strlen(chars)));
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
            runtimeError("Unsupported object type.");
            break;
    }
}

ObjString* objTypeName(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            return copyString("string", 6);
    }

    return copyString("unknown type", 12);
}

Value valueToString(Value value) {
    char buffer[32];

    if (IS_NULL(value)) {
        return stringValue("null");
    }

    if (IS_NUMBER(value)) {
        int length =
            snprintf(buffer, sizeof(buffer), "%.15g", AS_NUMBER(value));
        return OBJ_VAL(copyString(buffer, length));
    }

    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            return value;
    }

    return NULL_VAL;
}