#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <cpu.h>

typedef struct CPU{

    int quantum_time;
    int loop_ready;
    
} CPU;

CPU cpu;

void init_cpu(){
    loop_cpu();
}

void loop_cpu(){
    BCP *processo_rodando;
    while(cpu.loop_ready){

        time.sleep(1);
        cpu.quantuim_time++;
        inicializa_processos_ready(BCP);
        processo_rodando = retirar_processo(BCP);

        if(processo_rodando){

            executar_processo(processo_rodando);

        }
    }
    
}

void executar_processo(BCP *processo){

    
}