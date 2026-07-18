#ifndef neon_value_h
#define neon_value_h

#include <string.h>

#include "common.h"

typedef struct VM VM;
typedef struct Obj Obj;
typedef struct ObjString ObjString;

typedef enum { VAL_NULL, VAL_NUMBER, VAL_OBJ } ValueType;

#ifdef NAN_BOXING

/*
    values in Neon are stored using NaN-boxing.

    IEEE-754 doubles reserve a large range of NaN bit patterns.
    Neon uses those unused NaN bits to encode value types:
    - null
    - booleans
    - object pointers (assumes pointers fit within 48 bits)

    numbers remain as doubles.

    this allows every Value to be exactly 64 bits while avoiding unions
    and improves cache locality.
*/

#define SIGN_BIT ((uint64_t)0x8000000000000000)
// Quite NaN bits
#define QNAN ((uint64_t)0x7ffc000000000000)

// right most 2 bits of Value, to represent NULL
#define TAG_NULL 1       // 01
#define TAG_TOMBSTONE 2  // 10

typedef uint64_t Value;

#define NULL_VAL ((Value)(uint64_t)(QNAN | TAG_NULL))
#define NUMBER_VAL(num) numToValue(num)

/*
    assumes pointers fit within 48 bits.
    valid on mainstream x86-64 and ARM64 CPUs.
 */
#define OBJ_VAL(obj) (Value)(SIGN_BIT | QNAN | (uint64_t)(uintptr_t)(obj))

#define IS_NUMBER(value) (((value) & QNAN) != QNAN)
#define IS_NULL(value) ((value) == NULL_VAL)
#define IS_OBJ(value) (((value) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))

#define AS_NUMBER(value) valueToNum(value)
#define AS_OBJ(value) ((Obj*)(uintptr_t)((value) & ~(SIGN_BIT | QNAN)))

// a special value for the hash table
#define TOMBSTONE_VAL ((Value)(QNAN | TAG_TOMBSTONE))

#define IS_TOMBSTONE(v) ((v) == TOMBSTONE_VAL)

static inline double valueToNum(Value value) {
    /*
        type punning to convert Value to a double.
        this avoids aliasing issues and is optimized away by modern compilers.
    */
    double num;
    memcpy(&num, &value, sizeof(Value));
    return num;
}

static inline Value numToValue(double num) {
    /*
        type punning to convert a double to Value.
        this avoids aliasing issues and is optimized away by modern compilers.
    */
    Value value;
    memcpy(&value, &num, sizeof(value));
    return value;
}

#else
// Some cpu architectures might not support the NaN boxing implementation.
typedef struct {
    ValueType type;
    union {
        double number;
        Obj* obj;
    } as;
} Value;

#define NULL_VAL ((Value){VAL_NULL, {.number = 0}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})
#define OBJ_VAL(object) ((Value){VAL_OBJ, {.obj = (Obj*)object}})

#define IS_NULL(value) ((value).type == VAL_NULL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)
#define IS_OBJ(value) ((value).type == VAL_OBJ)

#define AS_NUMBER(value) ((value).as.number)
#define AS_OBJ(value) ((value).as.obj)
#endif

typedef struct {
    int capacity;
    int count;
    Value* values;
} ValueArray;

bool valuesEqual(Value a, Value b);

void initValueArray(ValueArray* array);
void initValueArrayWithCapacity(ValueArray* array, int capacity);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);
ObjString* valueTypeName(Value value);

#endif