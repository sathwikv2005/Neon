#ifndef neon_object_h
#define neon_object_h

#include "chunk.h"
#include "common.h"
#include "value.h"

#define OBJ_TYPE(value) AS_OBJ(value)->type
#define IS_STRING(value) isObjType(value, OBJ_STRING)
#define IS_LIST(value) isObjType(value, OBJ_LIST)

#define AS_LIST(value) ((ObjList*)AS_OBJ(value))
#define AS_STRING(value) ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString*)AS_OBJ(value))->chars)

typedef enum { OBJ_STRING, OBJ_LIST } ObjType;

struct Obj {
    ObjType type;
    bool survived;
    Obj* next;
};

struct ObjString {
    Obj obj;
    int length;
    char* chars;
    size_t ref;
    uint32_t hash;
};

typedef struct {
    Obj obj;
    ValueArray list;
} ObjList;

ObjString* takeString(char* chars, int length);
ObjString* copyString(const char* chars, int length);
void printObject(Value value);
Value valueToString(Value value);
void printlist(ObjList* list);

ObjString* objTypeName(Value value);

ObjList* newList(int capacity);

static inline bool isObjType(Value value, ObjType type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif