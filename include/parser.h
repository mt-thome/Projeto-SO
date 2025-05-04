#ifndef PARSER_H
#define PARSER_H
#define MAX_INSTR 100
#define MAX_NAME 32
#define MAX_SEM 10

#include "../include/bcp.h"
#include "../include/cpu.h"
#include "../include/memory.h"
#include "../include/scheduler.h"
#include "../include/events.h"

typedef struct Instruction
{
    char type[16];     // Instruction type
    int parameter;     // Numeric parameter
    char sem[8];  // Semaphore name (sized for 7 chars + null terminator)
    int pc; // Program counter
    int quantum_time; // Tempo de execução do instrução em ms
} instr;

BCP *load_program(const char *file_path, int next_id);

#endif
