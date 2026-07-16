#ifndef NEON_FILE_H
#define NEON_FILE_H

#include <stdio.h>

#include "common.h"

typedef struct {
    FILE* handle;
} File;

typedef enum {
    FILE_READ,
    FILE_WRITE,
    FILE_APPEND,
    FILE_READ_WRITE,
    FILE_WRITE_READ,
    FILE_APPEND_READ
} FileMode;

bool fileOpen(File* file, const char* path, FileMode mode);
void fileClose(File* file);

size_t fileRead(File* file, void* buffer, size_t size);
size_t fileWrite(File* file, const void* buffer, size_t size);

bool fileSeek(File* file, long offset, int origin);
long fileTell(File* file);
bool fileEof(File* file);

#endif