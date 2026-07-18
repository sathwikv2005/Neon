#ifndef neon_serializer_h
#define neon_serializer_h

#include "file.h"
#include "table.h"

#define SERIALIZER_MAGIC "NEON"
#define SERIALIZER_MAGIC_LEN 4
#define SERIALIZER_VERSION 1

bool writeValue(File* file, Value value);
bool readValue(File* file, Value* value);

bool writeKey(File* file, ObjString* key);
ObjString* readKey(File* file);

bool writeEntry(File* file, Entry* entry);
bool readEntry(File* file, Entry* entry);

bool writeTable(File* file, Table* table);
bool readTable(File* file, Table* table);

#endif