#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../include/cpu.h"

CPU cpu;
int pc;

void init_cpu(){
    loop_cpu();
    int pc = 0;
    cpu.quantum_time = 0;
    cpu.loop_ready = 1;
}

void loop_cpu(){
    BCP *processo_rodando;
    while(cpu.loop_ready){

        inicializa_processos_ready(get_bcp()); 
        if (processo_rodando && processo_rodando->instruction[processo_rodando->num_instr]->pc >= processo_rodando->num_instr) {
            end_process(processo_rodando); 
            processo_rodando = NULL;
        }

        if (!processo_rodando) {
            processo_rodando = retirar_processo(get_bcp());
        }

        if(processo_rodando){
            executar_processo(processo_rodando);
        }
    }
    
}

void executar_processo(BCP *processo){

    int time_slicing_local = time_slicing;
    int i=0;
    while(i<MAX_INSTR||time_slicing_local>0){
        if(processo->instruction[i]==0){ 
            i++;
            continue;
        }
        if(strcmp(processo->instruction[i]->type, "exec") == 0){
            if(processo->quantum_time > time_slicing_local){
                cpu.quantum_time += processo->instruction[i]->parameter;
                printf("\nExecutando o comando por %dms", time_slicing_local);
                processo->instruction[i]->parameter -= time_slicing_local;
                i++;
                continue;
            }
            usleep(processo->instruction[i]->parameter * 1000); // Use the parameter value
        } 
        else if(strcmp(processo->instruction[i]->type, "read") == 0){
            printf("\nLeitura por %dms", processo->instruction[i]->parameter);
            //Ideia de simulação de tempo de leitura
            //usleep(processo->instruction[i]->parameter * 2000); 
        }
        else if(strcmp(processo->instruction[i]->type, "write") == 0){
            printf("\nEscrita por %dms", processo->instruction[i]->parameter);
            // Ideia de simulação de tempo de escrita
            // usleep(processo->instruction[i]->parameter * 2000); // 
        }
        else if(strcmp(processo->instruction[i]->type, "P") == 0){
            printf("\nAcessando região crítica %d", atoi(processo->instruction[i]->sem));   
            // Simulação de tempo de espera do semáforo
            // usleep(50000); 50ms for semaphore operations
        }
        else if(strcmp(processo->instruction[i]->type, "V") == 0){
            printf("\nRegião crítica liberada");
            // Simulação de tempo de liberação do semáforo
            // usleep(50000); 
        }

        time_slicing_local--;

    }
    
    //kill programa
    if(i>= MAX_INSTR){

    }  

    if(time_slicing_local<=0){
        //chamar o próximo processo
        return loop_cpu();
    }

}

CPU get_cpu(){
    return cpu;
}

void set_cpu_pc(int new_pc){
    pc = new_pc;
}