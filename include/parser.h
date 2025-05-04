#ifndef PARSER_H
#define PARSER_H
#define MAX_INSTR 100
#define MAX_NAME 32
#define MAX_SEM 10

#include "../include/bcp.h"
#include "../include/cpu.h"

typedef struct Instruction
{
    char type[16];     // Instruction type
    int parameter;     // Numeric parameter
    char sem[8];  // Semaphore name (sized for 7 chars + null terminator)
    int pc; // Program counter
} instr;

BCP *load_program(const char *file_path, int next_id);

#endif
