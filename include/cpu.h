#ifndef CPU_H
#define CPU_H

#include "../include/bcp.h"
#include "../include/scheduler.h"
#include "../include/events.h"

#define time_slicing 100 // 100ms

typedef struct CPU{

    int quantum_time; // Tempo de execução do processo em ms
    int loop_ready;
    int pc; // Program counter
    
} CPU;

void init_cpu();
void loop_cpu();
void executar_processo(BCP *processo);
CPU get_cpu();
void set_cpu_pc(int pc);
void set_cpu_qt(int qt);

#endif