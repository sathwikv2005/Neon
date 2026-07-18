#include "serializer.h"

#include <limits.h>

#include "../include/memory.h"
#include "object.h"

static bool writeU8(File* file, uint8_t value) {
    return fileWrite(file, &value, sizeof(value)) == sizeof(value);
}

static bool writeU16(File* file, uint16_t value) {
    return fileWrite(file, &value, sizeof(value)) == sizeof(value);
}

static bool writeU32(File* file, uint32_t value) {
    return fileWrite(file, &value, sizeof(value)) == sizeof(value);
}

static bool writeU64(File* file, uint64_t value) {
    return fileWrite(file, &value, sizeof(value)) == sizeof(value);
}

static bool readU8(File* file, uint8_t* value) {
    return fileRead(file, value, sizeof(*value)) == sizeof(*value);
}

static bool readU16(File* file, uint16_t* value) {
    return fileRead(file, value, sizeof(*value)) == sizeof(*value);
}

static bool readU32(File* file, uint32_t* value) {
    return fileRead(file, value, sizeof(*value)) == sizeof(*value);
}

static bool readU64(File* file, uint64_t* value) {
    return fileRead(file, value, sizeof(*value)) == sizeof(*value);
}

static bool writeBytes(File* file, const void* data, size_t size) {
    return fileWrite(file, data, size) == size;
}

static bool readBytes(File* file, void* data, size_t size) {
    return fileRead(file, data, size) == size;
}

static bool writeMagic(File* file) {
    if (!writeBytes(file, SERIALIZER_MAGIC, SERIALIZER_MAGIC_LEN)) return false;

    uint8_t version = SERIALIZER_VERSION;
    return writeU8(file, version);
}

static bool readMagic(File* file) {
    char magic[SERIALIZER_MAGIC_LEN];

    if (fileRead(file, magic, SERIALIZER_MAGIC_LEN) != SERIALIZER_MAGIC_LEN)
        return false;

    if (memcmp(magic, SERIALIZER_MAGIC, SERIALIZER_MAGIC_LEN) != 0)
        return false;

    uint8_t version;
    if (!readU8(file, &version)) return false;

    return version == SERIALIZER_VERSION;
}

static bool writeSize(File* file, int size) {
    if (size < 0) return false;
    // the serializer expects 4 bytes. size of int is not guaranteed
    uint32_t value = (uint32_t)size;

    return writeU32(file, value);
}

static bool readSize(File* file, int* size) {
    uint32_t value;

    if (!readU32(file, &value)) return false;

    /*
        `int` could be smaller then 32bits. while rare, but allowed by the C
        standard
     */
    if (value > INT_MAX) return false;

    *size = (int)value;
    return true;
}

static bool writeString(File* file, ObjString* string) {
    uint32_t len = (uint32_t)(string->length);
    if (!writeU32(file, len)) {
        return false;
    }
    return writeBytes(file, string->chars, len);
}

static ObjString* readString(File* file) {
    uint32_t len;
    if (!readU32(file, &len)) return NULL;
    if (len == UINT32_MAX) return NULL;

    char* chars = ALLOCATE(char, len + 1);
    if (!readBytes(file, chars, len)) {
        FREE_ARRAY(char, chars, len + 1);
        return NULL;
    }
    chars[len] = '\0';

    return takeString(chars, len);
}

bool writeObj(File* file, Value value) {
    Obj* obj = AS_OBJ(value);

    if (!writeU8(file, obj->type)) return false;

    switch (obj->type) {
        case OBJ_STRING:
            return writeString(file, AS_STRING(value));
        default:
            break;
    }
    return false;
}

bool writeValue(File* file, Value value) {
    uint8_t tag;

    if (IS_OBJ(value)) {
        tag = VAL_OBJ;
    } else if (IS_NULL(value)) {
        tag = VAL_NULL;
    } else if (IS_NUMBER(value)) {
        tag = VAL_NUMBER;
    } else {
        return false;
    }

    if (!writeU8(file, tag)) return false;

    return tag == VAL_OBJ ? writeObj(file, value)
                          : writeU64(file, (uint64_t)value);
}

bool readValue(File* file, Value* value) {
    uint8_t tag;
    if (!readU8(file, &tag)) return false;

    switch (tag) {
        case VAL_NULL:
        case VAL_NUMBER: {
            uint64_t bits;
            if (!readU64(file, &bits)) return false;

            *value = (Value)bits;
            return true;
        }
        case VAL_OBJ:
            return readObj(file, value);
        default:
            return false;
    }

    return false;
}

bool writeKey(File* file, ObjString* key) { return writeString(file, key); }

ObjString* readKey(File* file) { return readString(file); }

bool writeEntry(File* file, Entry* entry) {
    if (entry->key == NULL) return false;

    return writeKey(file, entry->key) && writeValue(file, entry->value);
}

bool readEntry(File* file, Entry* entry) {
    // TODO
    return false;  // unimplemented
}

bool writeTable(File* file, Table* table) {
    if (!writeMagic(file)) {
        return false;
    }
    if (!writeSize(file, table->size)) {
        return false;
    }
    for (int i = 0; i < table->capacity; i++) {
        Entry* entry = &table->entries[i];

        if (entry->key == NULL) continue;

        if (!writeEntry(file, entry)) return false;
    }

    return true;
}

bool readTable(File* file, Table* table) {
    // TODO
    return false;  // unimplemented
}