#ifndef helium_table_h
#define helium_table_h

#include "common.h"
#include "value.h"

typedef struct {
    ObjString* key;
    Value value;
} Entry;

typedef struct {
    int size;   // live entries
    int count;  // size + tombstones
    int capacity;
    int mask;  // mask = capacity-1, used for fast index wrapping. (hash & mask)
    Entry* entries;
} Table;

void initTable(Table* table);
void freeTable(Table* table);
bool tableSet(Table* table, ObjString* key, Value value);
void tableAddAll(Table* from, Table* to);
bool tableGet(Table* table, ObjString* key, Value* value);
bool tableDelete(Table* table, ObjString* key);
bool tableRemove(Table* table, ObjString* key);
ObjString* tableFindString(Table* table, const char* chars, int length,
                           uint32_t hash);
Entry* tableEntries(Table* table);
// void markTable(Table* table);
// void tableRemoveWhite(Table* table);

#endif
