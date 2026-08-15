#include <stdio.h>

#include "compiler.h"
#include "debug.h"
#include "logger.h"
#include "vm_common.h"

// return exit back to the engine
#define INTERPRET_EXIT() ((InterpretOutput){INTERPRET_EXIT, false, NULL_VAL})

// return ok back to the engine
#define INTERPRET_OK() ((InterpretOutput){INTERPRET_OK, false, NULL_VAL})

// return ok with a result back to the engine
#define INTERPRET_RESULT(v) ((InterpretOutput){INTERPRET_OK, true, (v)})

// report an error back to the engine
#define ERROR_STATUS(s) ((InterpretOutput){(s), false, NULL_VAL})

void initvm(VM* vm) {
    resetStack(vm);
    // vm->objects = NULL;
    // vm->grayCount = 0;
    // vm->grayCapacity = 0;
    // vm->grayStack = NULL;
    vm->debugFlags = 0;
    vm->atLineStart = true;
    // vm->bytesAllocated = 0;
    // vm->nextGC = 1024 * 1024;
    // vm->currentGCMark = true;
    vm->ip = 0;
    vm->error = NULL;
    vm->errorCapacity = 0;
}

void freevm(VM* vm) {
    FREE_ARRAY(char, vm->error, vm->errorCapacity);
    vm->error = NULL;
    vm->errorCapacity = 0;
}

static InterpretOutput run(VM* vm) {
    const Chunk* chunk = vm->chunk;
    register uint8_t* ip = vm->ip;

/*
    The ip is cached locally so, that the c compiler can store it in a register
    for faster access hence, the ip must be written back to the vm so
    that the runtimeError() can report the correct source line.
*/
#define RUNTIME_ERROR(...)             \
    do {                               \
        vm->ip = ip;                   \
        runtimeError(vm, __VA_ARGS__); \
    } while (false)
#define PUSH(v) push(vm, (v))
#define POP() pop(vm)
#define PEEK(n) peek(vm, (n))
#define READ_BYTE() (*ip++)
#define READ_CONSTANT() (chunk->constants.values[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())

    while (true) {
#ifdef NEON_DEBUG
        if (GET_DEBUG_TRACE()) {
            printf("Stack=>\t");
            printf("[ ");
            for (Value* slot = vm->stack; slot < vm->stackTop; slot++) {
                printValue(*slot);
                printf(", ");
            }
            printf("]");
            printf("\n");
            disassembleInstruction(chunk, (int)(ip - chunk->code));
            // printf("==+++++++++==\n");
        }
#endif
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_SET: {
                ObjString* key = READ_STRING();
                Value value = READ_CONSTANT();
                tableSet(&vm->database->table, key, value);
                return INTERPRET_OK();
            }

            case OP_GET: {
                ObjString* key = READ_STRING();
                Value value;
                if (!tableGet(&vm->database->table, key, &value)) {
                    return INTERPRET_RESULT(NULL_VAL);
                }
                return INTERPRET_RESULT(value);
            }

            case OP_DEL: {
                ObjString* key = READ_STRING();
                if (!tableDelete(&vm->database->table, key)) {
                    RUNTIME_ERROR("Failed to delete %s", key->chars);
                    return ERROR_STATUS(INTERPRET_RUNTIME_ERROR);
                }
                return INTERPRET_OK();
            }

            case OP_INCR: {
                ObjString* key = READ_STRING();
                Value value;
                if (!tableGet(&vm->database->table, key, &value)) {
                    tableSet(&vm->database->table, key, NUMBER_VAL(1));
                    return INTERPRET_RESULT(NUMBER_VAL(1));
                }
                if (!IS_NUMBER(value)) {
                    RUNTIME_ERROR("Value must be a number.");
                }
                double num = AS_NUMBER(value);
                num++;
                tableSet(&vm->database->table, key, NUMBER_VAL(num));
                return INTERPRET_RESULT(NUMBER_VAL(num));
            }
            case OP_DECR: {
                ObjString* key = READ_STRING();
                Value value;
                if (!tableGet(&vm->database->table, key, &value)) {
                    tableSet(&vm->database->table, key, NUMBER_VAL(-1));
                    return INTERPRET_RESULT(NUMBER_VAL(-1));
                }
                if (!IS_NUMBER(value)) {
                    RUNTIME_ERROR("Value must be a number.");
                }
                double num = AS_NUMBER(value);
                num--;
                tableSet(&vm->database->table, key, NUMBER_VAL(num));
                return INTERPRET_RESULT(NUMBER_VAL(num));
            }

            case OP_KEYS: {
                Entry* entries = tableEntries(&vm->database->table);
                int size = vm->database->table.size;
                ObjList* list = newList(size);
                for (int i = 0; i < vm->database->table.capacity; i++) {
                    if (entries[i].key == NULL) continue;
                    writeValueArray(&list->list, OBJ_VAL(entries[i].key));
                }
                return INTERPRET_RESULT(OBJ_VAL(list));
            }

            case OP_SELECT: {
                Value idValue = READ_CONSTANT();
                if (!IS_NUMBER(idValue)) {
                    RUNTIME_ERROR("id must be an integer from 0 to %d",
                                  MAX_DATABASE - 1);
                }
                double id = AS_NUMBER(idValue);
                if (id < 0 || id >= MAX_DATABASE || id != (uint8_t)id) {
                    RUNTIME_ERROR("id must be an integer from 0 to %d",
                                  MAX_DATABASE - 1);
                }
                if (vm->database->id == (uint8_t)id) {
                    return INTERPRET_OK();
                }

                Database* newDatabase = loadDatabase((uint8_t)id);
                if (newDatabase == NULL) {
                    LOG_ERROR("Unable to load database(%u)", (unsigned)id);
                    RUNTIME_ERROR("Unable to load database(%u)", (unsigned)id);
                }
                Database* oldDatabase = vm->database;
                vm->database = newDatabase;
                oldDatabase->clients--;
                unloadDatabase(oldDatabase);

                return INTERPRET_OK();
            }
            case OP_DBSIZE: {
                Value value = NUMBER_VAL((double)vm->database->table.size);
                return INTERPRET_RESULT(value);
            }

            case OP_FLUSH: {
                freeTable(&vm->database->table);
                return INTERPRET_OK();
            }

            case OP_SAVE: {
                if (!saveDatabase(vm->database)) {
                    RUNTIME_ERROR(
                        "Failed to save database. Check the log file for more "
                        "info.");
                }
                return INTERPRET_OK();
            }

            case OP_TYPE: {
                ObjString* key = READ_STRING();
                Value value = NULL_VAL;

                tableGet(&vm->database->table, key, &value);

                return INTERPRET_RESULT(OBJ_VAL(valueTypeName(value)));
            }
            case OP_EXISTS: {
                uint8_t count = READ_BYTE();
                double result = 0;
                Value v;
                while (count--) {
                    if (tableGet(&vm->database->table, READ_STRING(), &v)) {
                        result++;
                    }
                }
                return INTERPRET_RESULT(NUMBER_VAL(result));
            }
            case OP_RENAME: {
                ObjString* key = READ_STRING();
                ObjString* newKey = READ_STRING();
                Value value = NULL_VAL;

                if (!tableGet(&vm->database->table, key, &value)) {
                    RUNTIME_ERROR("Failed to rename %s. It does not exist.",
                                  key->chars);
                }
                if (!tableSet(&vm->database->table, newKey, value)) {
                    LOG_ERROR("setting key %s failed", newKey->chars);
                    RUNTIME_ERROR("Failed to rename %s to %s.", key->chars,
                                  newKey->chars);
                }
                if (!tableDelete(&vm->database->table, key)) {
                    LOG_ERROR("Deleting key %s failed", key->chars);
                    RUNTIME_ERROR("Failed to rename %s to %s.", key->chars,
                                  newKey->chars);
                }

                return INTERPRET_OK();
            }

            case OP_RETURN:
                return INTERPRET_OK();

            case OP_PING: {
                // TODO: maybe should return "PONG" in the furture.
                //  to do that will need to intern "PONG" at startup as a
                //  special string
                return INTERPRET_OK();
            }
            case OP_ECHO: {
                return INTERPRET_RESULT(READ_CONSTANT());
            }
            case OP_EXIT:
                return INTERPRET_EXIT();
            default:
                break;
        }
    }
    return ERROR_STATUS(INTERPRET_RUNTIME_ERROR);
}

InterpretOutput interpret(const char* source, VM* vm) {
    switch (setjmp(vm->vmJmp)) {
        case JUMP_RUNTIME_ERROR:
            return ERROR_STATUS(INTERPRET_RUNTIME_ERROR);
        case JUMP_EXIT:
            return ERROR_STATUS(INTERPRET_EXIT);
    }
    Chunk chunk;
    initChunk(&chunk);
    if (!compile(vm, source, &chunk)) {
        return ERROR_STATUS(INTERPRET_COMPILE_ERROR);
    }

    vm->chunk = &chunk;
    vm->ip = chunk.code;

    InterpretOutput result = run(vm);

    freeChunk(&chunk);

    return result;
}