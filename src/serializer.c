#include "serializer.h"

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

static bool writeCapacity(File* file, int capacity) {
    // the serializer expects 4 bytes. size of int is not guaranteed
    uint32_t value = (uint32_t)capacity;

    return writeU32(file, value);
}

bool writeValue(File* file, Value value) {
    // TODO
    return false;  // unimplemented
}

bool readValue(File* file, Value* value) {
    // TODO
    return false;  // unimplemented
}

bool writeKey(File* file, ObjString* key) {
    // TODO
    return false;  // unimplemented
}

bool readKey(File* file, ObjString** key) {
    // TODO
    return false;  // unimplemented
}

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
    if (!writeCapacity(file, table->capacity)) {
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