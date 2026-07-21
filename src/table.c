#include "table.h"

#include <stdlib.h>
#include <string.h>

#include "../include/memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define TABLE_MAX_LOAD 0.75

void initTable(Table* table) {
    table->size = 0;
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
    table->mask = 0;
}

static void freeEntry(Entry* entry) {
    if (entry->key == NULL) return;
    releaseString(entry->key);
    if (IS_OBJ(entry->value)) releaseObject(AS_OBJ(entry->value));
}

static void freeEntries(Table* table) {
    Entry* entries = table->entries;
    for (int i = 0; i < table->capacity; i++) {
        freeEntry(&entries[i]);
    }
}

void freeTable(Table* table) {
    freeEntries(table);
    FREE_ARRAY(Entry, table->entries, table->capacity);
    initTable(table);
}

static Entry* findEntry(Entry* entries, int mask, ObjString* key) {
    // faster than (hash % capacity) since capacity is always a power of 2.
    // mask = capacity - 1 for fast index wrapping.
    uint32_t index = key->hash & mask;

    Entry* tombstone = NULL;

    for (;;) {
        Entry* entry = &entries[index];

        if (entry->key == NULL) {
            if (IS_NULL(entry->value)) {
                return tombstone != NULL ? tombstone : entry;
            }

            if (tombstone == NULL) tombstone = entry;
        } else if (entry->key == key) {
            return entry;
        }
        index = (index + 1) & mask;
    }
}

bool tableGet(Table* table, ObjString* key, Value* value) {
    if (table->count == 0) return false;

    Entry* entry = findEntry(table->entries, table->mask, key);
    if (entry->key == NULL) return false;

    *value = entry->value;
    return true;
}

static void adjustCapacity(Table* table, int capacity) {
    Entry* entries = ALLOCATE(Entry, capacity);
    int mask = capacity - 1;
    for (int i = 0; i < capacity; i++) {
        entries[i].key = NULL;
        entries[i].value = NULL_VAL;
    }
    table->count = 0;
    table->size = 0;
    for (int i = 0; i < table->capacity; i++) {
        Entry* entry = &table->entries[i];
        if (entry->key == NULL) continue;

        Entry* newEntry = findEntry(entries, mask, entry->key);
        newEntry->key = entry->key;
        newEntry->value = entry->value;
        table->count++;
        table->size++;
    }
    FREE_ARRAY(Entry, table->entries, table->capacity);
    table->entries = entries;
    table->capacity = capacity;
    table->mask = mask;
}

bool tableSet(Table* table, ObjString* key, Value value) {
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
        if (table->size < table->capacity / 2)
            adjustCapacity(table, table->capacity);  // clear tombstones
        else
            adjustCapacity(table, GROW_CAPACITY(table->capacity));
    }

    Entry* entry = findEntry(table->entries, table->mask, key);

    bool isNewKey = entry->key == NULL;
    if (isNewKey) {
        table->size++;
        retainString(key);
        if (IS_NULL(entry->value)) {
            table->count++;
        }
    }
    if (!isNewKey && IS_OBJ(entry->value)) releaseObject(AS_OBJ(entry->value));

    entry->key = key;
    entry->value = value;
    if (IS_OBJ(value)) retainObject(AS_OBJ(value));

    return isNewKey;
}

void tableAddAll(Table* from, Table* to) {
    for (int i = 0; i < from->capacity; i++) {
        Entry* entry = &from->entries[i];
        if (entry->key == NULL) continue;

        tableSet(to, entry->key, entry->value);
    }
}

ObjString* tableFindString(Table* table, const char* chars, int length,
                           uint32_t hash) {
    if (table->count == 0) return NULL;

    uint32_t index = hash & table->mask;

    for (;;) {
        Entry* entry = &table->entries[index];
        if (entry->key == NULL) {
            if (IS_NULL(entry->value)) return NULL;
            // tombstone, continue probing
        } else if (entry->key->length == length && entry->key->hash == hash &&
                   memcmp(entry->key->chars, chars, length) == 0) {
            return entry->key;
        }
        index = (index + 1) & table->mask;
    }
}

/*
    clears an entry from the table but does not free memory
*/
bool tableRemove(Table* table, ObjString* key) {
    if (table->count == 0) return false;
    Entry* entry = findEntry(table->entries, table->mask, key);
    if (entry->key == NULL) return false;

    table->size--;
    entry->key = NULL;
    entry->value = TOMBSTONE_VAL;
    return true;
}

bool tableDelete(Table* table, ObjString* key) {
    if (table->count == 0) return false;
    Entry* entry = findEntry(table->entries, table->mask, key);
    if (entry->key == NULL) return false;

    freeEntry(entry);

    table->size--;
    entry->key = NULL;
    entry->value = TOMBSTONE_VAL;

    return true;
}

Entry* tableEntries(Table* table) { return table->entries; }

// void markTable(Table* table) {
//     for (int i = 0; i < table->capacity; i++) {
//         Entry* entry = &table->entries[i];
//         markObject((Obj*)entry->key);
//         markValue(entry->value);
//     }
// }
// void tableRemoveWhite(Table* table) {
//     for (int i = 0; i < table->capacity; i++) {
//         Entry* entry = &table->entries[i];
//         if (entry->key != NULL &&
//             entry->key->obj.isMarked != vm.currentGCMark) {
//             tableDelete(table, entry->key);
//         }
//     }
// }