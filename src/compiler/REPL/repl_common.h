#include "../compiler_common.h"

// parser
void synchronize(Compiler* compiler);
void advance(Compiler* compiler);
void consume(Compiler* compiler, NeonTokenType type, const char* message);
bool check(Parser* parser, NeonTokenType type);
bool match(Compiler* compiler, NeonTokenType type);

// compiler
void parseKey(Compiler* compiler, const char* message);
void parseValue(Compiler* compiler, const char* message);
Chunk* currentChunk(Compiler* compiler);

// util
void errorAt(Compiler* compiler, Token* token, const char* fmt, ...);
void error(Compiler* compiler, const char* message);
void errorAtCurrent(Compiler* compiler, const char* message);
uint8_t makeConstant(Compiler* compiler, Value value);
void emitByte(Compiler* compiler, uint8_t byte);
void emitBytes(Compiler* compiler, uint8_t byte1, uint8_t byte2);
int emitPlaceHolderByte(Compiler* compiler);
void patchByte(Compiler* compiler, int offset, uint8_t newByte);

// commands
typedef void (*CommandFn)(Compiler*);
extern CommandFn commandTable[];
extern const size_t commandTableSize;
