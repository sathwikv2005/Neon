#include "file.h"

static const char* fileModeString(FileMode mode) {
    switch (mode) {
        case FILE_READ:
            return "rb";
        case FILE_WRITE:
            return "wb";
        case FILE_APPEND:
            return "ab";
        case FILE_READ_WRITE:
            return "rb+";
        case FILE_WRITE_READ:
            return "wb+";
        case FILE_APPEND_READ:
            return "ab+";
    }

    return NULL;
}

bool fileOpen(File* file, const char* path, FileMode mode) {
    const char* modeString = fileModeString(mode);
    if (modeString == NULL) return false;

    file->handle = fopen(path, modeString);
    return file->handle != NULL;
}

void fileClose(File* file) {
    if (file->handle == NULL) return;

    fclose(file->handle);
    file->handle = NULL;
}

size_t fileRead(File* file, void* buffer, size_t size) {
    return fread(buffer, 1, size, file->handle);
}

size_t fileWrite(File* file, const void* buffer, size_t size) {
    return fwrite(buffer, 1, size, file->handle);
}

bool fileSeek(File* file, long offset, int origin) {
    return fseek(file->handle, offset, origin) == 0;
}

long fileTell(File* file) { return ftell(file->handle); }

bool fileEof(File* file) { return feof(file->handle); }