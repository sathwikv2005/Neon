#include "value.h"

#include <stdio.h>
#include <string.h>

// use a relative path to avoid conflicting with the system <memory.h>
#include "../include/memory.h"
#include "object.h"

void initValueArray(ValueArray* array) {
    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}

void initValueArrayWithCapacity(ValueArray* array, int capacity) {
    array->capacity = capacity;
    array->count = 0;
    array->values = ALLOCATE(Value, capacity);
}

void writeValueArray(ValueArray* array, Value value) {
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values =
            GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

void freeValueArray(ValueArray* array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);
}

void printValue(Value value) {
#ifdef NAN_BOXING
    if (IS_NULL(value)) {
        printf("null");
    } else if (IS_NUMBER(value)) {
        printf("%.15g", AS_NUMBER(value));
    } else if (IS_OBJ(value)) {
        printObject(value);
    }
#else
    switch (value.type) {
        case VAL_NULL:
            printf("null");
            break;
        case VAL_NUMBER:
            printf("%.15g", AS_NUMBER(value));
            break;
        case VAL_OBJ:
            printObject(value);
            break;
    }
#endif
}

bool valuesEqual(Value a, Value b) {
#ifdef NAN_BOXING
    // To preserve IEEE-754 standard for NAN comparisons
    // NAN == NAN => false
    if (IS_NUMBER(a) && IS_NUMBER(b)) {
        return AS_NUMBER(a) == AS_NUMBER(b);
    }
    return a == b;
#else
    if (a.type != b.type) return false;

    switch (a.type) {
        case VAL_NULL:
            return true;
        case VAL_NUMBER:
            return AS_NUMBER(a) == AS_NUMBER(b);
        case VAL_OBJ: {
            return AS_OBJ(a) == AS_OBJ(b);
        }
        default:
            return false;
    }
#endif
}

ObjString* valueTypeName(VM* vm, Value value) {
#ifdef NAN_BOXING
    if (IS_NULL(value)) {
        return copyString(vm, "null", 4);
    }

    if (IS_NUMBER(value)) {
        return copyString(vm, "number", 6);
    }

    if (IS_OBJ(value)) {
        return objTypeName(vm, value);
    }
#else
    switch (value.type) {
        case VAL_NULL:
            return copyString(vm, "null", 4);

        case VAL_NUMBER:
            return copyString(vm, "number", 6);

        case VAL_OBJ:
            return objTypeName(vm, value);
    }
#endif

    return copyString(vm, "unknown type", 12);
}