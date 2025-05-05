#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>

#include "../include/bcp.h"
#include "../include/cpu.h"
#include "../include/memory.h"
#include "../include/parser.h"
#include "../include/events.h"
#include "../include/scheduler.h"
#include "../include/interface.h"

int main() {
    int running = 1;
    
    // Inicializa os subsistemas
    mem_init(get_memory_manager());
    init_bcp();
    init_cpu();
    
    while (running) {
       // Lê o arquivo de entrada
       BCP *process = load_program("resources/synthetic_2.txt", 0);
       if (process == NULL) {
           fprintf(stderr, "Erro ao carregar o programa.\n");
           break;
       }

       // Adiciona o processo à lista de processos
       new_process(NULL, process);

       // Inicia o loop da CPU
       //loop_cpu();
       // Mostra a interface
       if(show_menu()) {
           running = 0; // Sai do loop se a opção de sair for escolhida
       }
       
    }    

    return 0;
}