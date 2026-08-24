#ifndef neon_compiler_common_h
#define neon_compiler_common_h

#include "compiler.h"
#include "object.h"
#include "parser.h"
#include "scanner.h"
#include "value.h"

// commands
typedef void (*CommandFn)(Compiler*);
extern CommandFn commandTable[];
extern const size_t commandTableSize;

#endif