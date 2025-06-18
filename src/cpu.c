#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include "../include/cpu.h"
#include "../include/scheduler.h"
#include "../include/bcp.h"
CPU cpu;
int pc;

void init_cpu(){
    cpu.pc = 0;
    cpu.quantum_time = 0;
    cpu.loop_ready = 1;
    int lock = create_semaphore("IO", 1);
    if(lock < 0) {
        fprintf(stderr, "Erro ao criar semáforo para IO: %d\n", lock);
    } else {
        printf("Semáforo IO criado com sucesso.\n");
    }
}

void loop_cpu(){
    BCP *processo_rodando = get_bcp();
        inicializar_processos_ready(get_bcp() , 0); 
        if (!processo_rodando)
            processo_rodando = retirar_processo(get_bcp());

        if(processo_rodando)
            executar_processo(processo_rodando);
}

void executar_processo(BCP *processo){
    int i=processo->instr_index;

    if(strcmp(processo->instruction[i]->type, "exec") == 0){
        sys_call(PROCESS_RUN, processo, NULL, 0);
        inicializar_processos_ready(get_bcp(), 0);
    }

    else if(strcmp(processo->instruction[i]->type, "read") == 0){
        sys_call(DISK_REQUEST, processo, NULL, 0);
    }
    else if(strcmp(processo->instruction[i]->type, "write") == 0){
        printf("\nEscrita por %dms", processo->instruction[i]->parameter);
        sys_call(DISK_REQUEST, processo, NULL, 0);
    }
    else if(strcmp(processo->instruction[i]->type, "P") == 0){
        printf("\nRegião crítica solicitada");
        sys_call(SEMAPHORE_P, processo, processo->instruction[i]->sem, i);
    }
    else if(strcmp(processo->instruction[i]->type, "V") == 0){
        printf("\nRegião crítica liberada");
        sys_call(SEMAPHORE_V, processo, processo->instruction[i]->sem, i); 
    }
    else{
        processo->instr_index++;
        executar_processo(processo);
    }
}
    
CPU get_cpu(){
    return cpu;
}

int get_cpu_pc(){
    return pc;
}

void set_cpu_pc(int new_pc){
    pc = new_pc;
}

void set_cpu_qt(int qt){
    cpu.quantum_time = qt;
}