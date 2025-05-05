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
}

void loop_cpu(){
    BCP *processo_rodando = get_bcp();


        inicializar_processos_ready(get_bcp() , 0); 

        // if (processo_rodando && processo_rodando->instruction[processo_rodando->num_instr]->pc >= processo_rodando->num_instr) {
        //     printf("\n1");
        //     end_process(processo_rodando); 
        //     processo_rodando = NULL;
        // }

        if (!processo_rodando) {

            processo_rodando = retirar_processo(get_bcp());
        }

        if(processo_rodando){

            executar_processo(processo_rodando);
        }
    
}

void executar_processo(BCP *processo){
    int i=processo->instr_index;
    //  while(i < MAX_INSTR || get_cpu().quantum_time > 0){
    //     if(processo->quantum_time<=0){
    //         i++;
    //         continue;
    //     }
    if(strcmp(processo->instruction[i]->type, "exec") == 0){

        sys_call(PROCESS_RUN, processo, NULL, 0);
        inicializar_processos_ready(get_bcp(), 0);
        //set_cpu_qt(get_cpu().quantum_time-processo->quantum_time);
    } 
        else if(strcmp(processo->instruction[i]->type, "read") == 0){
            sys_call(DISK_REQUEST, processo, NULL, 0);
        }
        else if(strcmp(processo->instruction[i]->type, "write") == 0){
            printf("\nEscrita por %dms", processo->instruction[i]->parameter);
            sys_call(DISK_REQUEST, processo, NULL, 0);

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
        else
        {
            processo->instr_index++;
            executar_processo(processo);
        }
        
     }
    // set_cpu_qt(time_slicing);
    
    // //kill programa
    // if(i>= MAX_INSTR){
    //     printf("\nProcesso %d finalizado", processo->id);
    //     sys_call(PROCESS_FINISH, processo, NULL, 0);
    //     return;
    // }  

    // if(get_cpu().quantum_time<=0){
    //     //chamar o próximo processo
    //     return loop_cpu();
    // }

//}

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