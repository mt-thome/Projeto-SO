#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <cpu.h>
#define time_slicing 500
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

    int time_slicing_local = time_slicing;
    int i=0;
    while(i<MAX_INSTR||time_slicing_local>0){
        if(processo->instruction[i]==0){
            i++;
            continue;
        }
        if(strcmp(processo->instruction,"exec"==0)){
            printf("\nExecutando o comando por %dms",processo->instruction_time[i]);

        }else if(strcmp(processo->instruction,"read"==0)){
            printf("\nLeitura por %dms",processo->instruction_time);
            //fazer algo com a leitura
        }
        else if(strcmp(processo->instruction,"write"==0)){
            printf("\nLeitura por %dms",processo->instruction_time);
            //fazer algo com a escrita
        }
        else if(strcmp(processo->instruction,"write"==0)){
            printf("\nLeitura por %dms",processo->instruction_time);
            //fazer algo com a escrita
        }else if(strcmp(processo->instruction,"P"==0)){
            printf("\nAcessando região crítica");
            //fazer algo com isso
        }else if(strcmp(processo->instruction,"V"==0)){
            printf("\nRegião crítica liberada");
            //fazer algo com a escrita
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