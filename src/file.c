#include "file.h"

#include <errno.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define PATH_SEP '\\'
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define PATH_SEP '/'
#define MKDIR(path) mkdir(path, 0755)
#endif

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

static bool createDirectories(const char* path) {
    char temp[1024];
    size_t len = strlen(path);

    if (len >= sizeof(temp)) return false;

    strcpy(temp, path);

    for (char* p = temp + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            char c = *p;
            *p = '\0';

            if (strlen(temp) > 0) {
                if (MKDIR(temp) != 0 && errno != EEXIST) return false;
            }

            *p = c;
        }
    }

    return true;
}

bool fileOpen(File* file, const char* path, FileMode mode) {
    const char* modeString = fileModeString(mode);
    if (modeString == NULL) return false;

    if (mode != FILE_READ) {
        if (!createDirectories(path)) return false;
    }

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