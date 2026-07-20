#include "debug.h"

#include <stdio.h>

#include "line.h"
#include "object.h"
#include "value.h"

void disassembleChunk(const Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);
    for (int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
    printf("==+++++++++==\n");
}

static int constantInstruction(const char* name, const Chunk* chunk,
                               int offset) {
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s [%4d] => '", name, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");
    return offset + 2;
}

static int simpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

static int byteInstruction(const char* name, const Chunk* chunk, int offset) {
    uint8_t slot = chunk->code[offset + 1];
    printf("%-16s [%4d]\n", name, slot);
    return offset + 2;
}

static int jumpInstruction(const char* name, int sign, const Chunk* chunk,
                           int offset) {
    uint16_t jump = (uint16_t)(chunk->code[offset + 1] << 8);
    jump |= chunk->code[offset + 2];
    printf("%-16s [%4d] => %d\n", name, offset, offset + 3 + sign * jump);
    return offset + 3;
}

static int invokeInstruction(const char* name, const Chunk* chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1];
    uint8_t argCount = chunk->code[offset + 2];
    printf("%-16s (%d args) %4d '", name, argCount, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");
    return offset + 3;
}

int disassembleInstruction(const Chunk* chunk, int offset) {
    printf("----%04d\t", offset);
    int line = getLine(chunk, offset);
    if (offset > 0 && line == getLine(chunk, offset - 1))
        printf("   |\t");
    else
        printf("%4d\t", line);
    uint8_t instruction = chunk->code[offset];

    switch (instruction) {
        case OP_PING:
            return simpleInstruction("OP_PING", offset);

        case OP_ECHO:
            return constantInstruction("OP_ECHO", chunk, offset);

        case OP_QUIT:
            return simpleInstruction("OP_QUIT", offset);

        case OP_SET:
            return constantInstruction("OP_SET", chunk, offset);

        case OP_GET:
            return constantInstruction("OP_GET", chunk, offset);

        case OP_DEL:
            return constantInstruction("OP_DEL", chunk, offset);

        case OP_KEYS:
            return simpleInstruction("OP_KEYS", offset);

        case OP_RENAME:
            return constantInstruction("OP_RENAME", chunk, offset);

        case OP_EXISTS:
            return constantInstruction("OP_EXISTS", chunk, offset);

        case OP_TYPE:
            return constantInstruction("OP_TYPE", chunk, offset);

        case OP_FLUSH:
            return simpleInstruction("OP_FLUSH", offset);

        case OP_DBSIZE:
            return simpleInstruction("OP_DBSIZE", offset);

        case OP_EXIT:
            return simpleInstruction("OP_EXIT", offset);

        case OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);

        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}
