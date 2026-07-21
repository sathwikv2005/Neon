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

static ObjString* allocateString(char* chars, int length, uint32_t hash) {
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
ObjString* takeString(char* chars, int length) {
    uint32_t hash = hashString(chars, length);

    ObjString* interned = tableFindString(&server.strings, chars, length, hash);

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

    ObjString* interned = tableFindString(&server.strings, chars, length, hash);
    if (interned != NULL) {
        return interned;
    }

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

void printListValue(Value value) {
    if (IS_STRING(value)) {
        printString(AS_CSTRING(value), true);
        return;
    }

    printValue(value);
}

void printList(ObjList* list) {
    printf("[ ");
    if (list->list.count == 0) {
        printf("]");
        return;
    }
    Value* values = list->list.values;
    int i;
    for (i = 0; i < list->list.count - 1; i++) {
        printListValue(values[i]);
        printf(", ");
    }
    printListValue(values[i]);
    printf(" ]");
}

void printObject(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printString(AS_CSTRING(value), false);
            break;
        case OBJ_LIST:
            printList(AS_LIST(value));
            break;
        default:
            break;
    }
}

ObjString* objTypeName(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            return copyString("string", 6);
        case OBJ_LIST:
            return copyString("list", 4);
    }

    return copyString("unknown type", 12);
}

static Value listToString(ObjList* list) {
    if (list->list.count == 0) {
        return OBJ_VAL(copyString("[]", 2));
    }

    int capacity = 64;
    int length = 0;
    char* chars = ALLOCATE(char, capacity);

    chars[length++] = '[';

    for (int i = 0; i < list->list.count; i++) {
        Value value = list->list.values[i];

        ObjString* string = AS_STRING(valueToString(value));

        int strLength = string->length;

        while (length + strLength + 3 >= capacity) {
            int oldCapacity = capacity;
            capacity *= 2;
            chars = GROW_ARRAY(char, chars, oldCapacity, capacity);
        }

        memcpy(chars + length, string->chars, strLength);
        length += strLength;

        if (i != list->list.count - 1) {
            chars[length++] = ',';
            chars[length++] = ' ';
        }
    }

    chars[length++] = ']';
    chars[length] = '\0';

    ObjString* result = takeString(chars, length);
    return OBJ_VAL(result);
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
        case OBJ_LIST:
            return listToString(AS_LIST(value));
    }

    return NULL_VAL;
}

ObjList* newList(int capacity) {
    ObjList* list = ALLOCATE_OBJ(ObjList, OBJ_LIST);
    initValueArrayWithCapacity(&list->list, capacity);
    return list;
}