#ifndef PARSER_H
#define PARSER_H
#define MAX_INSTR 100
#define MAX_NAME 32
#define MAX_SEM 10

#include "bcp.h"

typedef struct Instruction
{
    char tipo[16];     // Instruction type
    int parametro;     // Numeric parameter
    char semaforo[8];  // Semaphore name (sized for 7 chars + null terminator)
} instr;

BCP *load_program(const char *arquivo);

#endif
