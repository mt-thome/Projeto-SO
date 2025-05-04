#ifndef CPU_H
#define CPU_H

#include "../include/bcp.h"
#include "../include/scheduler.h"

#define time_slicing 500

typedef struct CPU{

    int quantum_time;
    int loop_ready;
    
} CPU;

void init_cpu();
void loop_cpu();
void executar_processo(BCP *processo);
int get_cpu_pc();
void set_cpu_pc(int pc);

#endif