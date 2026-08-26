#include <string.h>

#include "../../../include/memory.h"
#include "resp_common.h"

static bool findCRLF(const char* data, size_t length, size_t start,
                     size_t* end) {
    for (size_t i = start; i + 1 < length; i++) {
        if (data[i] == '\r' && data[i + 1] == '\n') {
            *end = i;
            return true;
        }
    }

    return false;
}

static bool parseInteger(const char* data, size_t length, int64_t* result) {
    if (length == 0) return false;

    char buffer[64];

    if (length >= sizeof(buffer)) return false;

    memcpy(buffer, data, length);
    buffer[length] = '\0';

    char* end;
    long long value = strtoll(buffer, &end, 10);

    if (*end != '\0') return false;

    *result = value;
    return true;
}

static RespParseResult parseValue(const char* data, size_t length,
                                  size_t* offset, RespValue* value) {
    if (*offset >= length) return RESP_PARSE_INCOMPLETE;

    char prefix = data[*offset];
    (*offset)++;

    size_t lineEnd;

    switch (prefix) {
        case '+': {
            if (!findCRLF(data, length, *offset, &lineEnd))
                return RESP_PARSE_INCOMPLETE;

            size_t stringLength = lineEnd - *offset;

            value->type = RESP_SIMPLE_STRING;
            value->string.data = ALLOCATE(char, stringLength + 1);

            if (!value->string.data) return RESP_PARSE_ERROR;

            memcpy(value->string.data, data + *offset, stringLength);

            value->string.data[stringLength] = '\0';
            value->string.length = stringLength;

            *offset = lineEnd + 2;

            return RESP_PARSE_OK;
        }

        case '-': {
            if (!findCRLF(data, length, *offset, &lineEnd))
                return RESP_PARSE_INCOMPLETE;

            size_t stringLength = lineEnd - *offset;

            value->type = RESP_ERROR;
            value->string.data = ALLOCATE(char, stringLength + 1);

            if (!value->string.data) return RESP_PARSE_ERROR;

            memcpy(value->string.data, data + *offset, stringLength);

            value->string.data[stringLength] = '\0';
            value->string.length = stringLength;

            *offset = lineEnd + 2;

            return RESP_PARSE_OK;
        }

        case ':': {
            if (!findCRLF(data, length, *offset, &lineEnd))
                return RESP_PARSE_INCOMPLETE;

            if (!parseInteger(data + *offset, lineEnd - *offset,
                              &value->integer)) {
                return RESP_PARSE_ERROR;
            }

            value->type = RESP_INTEGER;
            *offset = lineEnd + 2;

            return RESP_PARSE_OK;
        }

        case '$': {
            if (!findCRLF(data, length, *offset, &lineEnd))
                return RESP_PARSE_INCOMPLETE;

            int64_t stringLength;

            if (!parseInteger(data + *offset, lineEnd - *offset,
                              &stringLength)) {
                return RESP_PARSE_ERROR;
            }

            *offset = lineEnd + 2;

            if (stringLength == -1) {
                value->type = RESP_NULL;
                return RESP_PARSE_OK;
            }

            if (stringLength < 0) return RESP_PARSE_ERROR;

            size_t required = (size_t)stringLength + 2;

            if (length - *offset < required) return RESP_PARSE_INCOMPLETE;

            if (data[*offset + stringLength] != '\r' ||
                data[*offset + stringLength + 1] != '\n') {
                return RESP_PARSE_ERROR;
            }

            value->type = RESP_BULK_STRING;

            value->string.data = ALLOCATE(char, (size_t)stringLength + 1);

            if (!value->string.data) return RESP_PARSE_ERROR;

            memcpy(value->string.data, data + *offset, (size_t)stringLength);

            value->string.data[stringLength] = '\0';
            value->string.length = (size_t)stringLength;

            *offset += required;

            return RESP_PARSE_OK;
        }

        case '*': {
            if (!findCRLF(data, length, *offset, &lineEnd))
                return RESP_PARSE_INCOMPLETE;

            int64_t count;

            if (!parseInteger(data + *offset, lineEnd - *offset, &count)) {
                return RESP_PARSE_ERROR;
            }

            *offset = lineEnd + 2;

            if (count == -1) {
                value->type = RESP_NULL;
                return RESP_PARSE_OK;
            }

            if (count < 0) return RESP_PARSE_ERROR;

            value->type = RESP_ARRAY;
            value->array.count = (size_t)count;

            value->array.values = ALLOCATE(RespValue, (size_t)count);

            if (!value->array.values) return RESP_PARSE_ERROR;

            /*
             * ALLOCATE() does not zero-initialize memory,
             * unlike calloc(), so initialize the array ourselves.
             */
            memset(value->array.values, 0, sizeof(RespValue) * (size_t)count);

            for (size_t i = 0; i < (size_t)count; i++) {
                RespParseResult result =
                    parseValue(data, length, offset, &value->array.values[i]);

                if (result != RESP_PARSE_OK) {
                    freeRespValue(value);
                    return result;
                }
            }

            return RESP_PARSE_OK;
        }

        default:
            return RESP_PARSE_ERROR;
    }
}

RespParseResult parseResp(const char* data, size_t length, RespValue* value) {
    memset(value, 0, sizeof(*value));

    size_t offset = 0;

    return parseValue(data, length, &offset, value);
}

void freeRespValue(RespValue* value) {
    if (!value) return;

    switch (value->type) {
        case RESP_SIMPLE_STRING:
        case RESP_ERROR:
        case RESP_BULK_STRING:
            FREE_ARRAY(char, value->string.data, value->string.length + 1);
            break;

        case RESP_ARRAY:
            for (size_t i = 0; i < value->array.count; i++) {
                freeRespValue(&value->array.values[i]);
            }

            FREE_ARRAY(RespValue, value->array.values, value->array.count);
            break;

        default:
            break;
    }

    memset(value, 0, sizeof(*value));
}