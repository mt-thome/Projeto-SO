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

jmp_buf exit_buffer;

// Função substituta para exit()
void safe_exit(int status) {
    longjmp(exit_buffer, status + 1); // +1 porque 0 é reservado
}

int main() {
    int running = 1;
    
    // Configura o ponto de retorno
    int exit_status = setjmp(exit_buffer);
    if (exit_status > 0) {
        printf("Uma função tentou chamar exit(%d)\n", exit_status - 1);
    }
    
    // Inicializa os subsistemas
    init_mem();
    init_bcp();
    init_cpu();
    
    while (running) {
        // Lê o arquivo de entrada
        BCP *process = load_program("program.txt", 0);
        if (process == NULL) {
            fprintf(stderr, "Erro ao carregar o programa.\n");
            break;
        }

        // Adiciona o processo à lista de processos
        new_process("program.txt", process);

        // Inicia o loop da CPU
        loop_cpu();
        // Mostra a interface
        if(show_interface()) {
            running = 0; // Sai do loop se a opção de sair for escolhida
        }
    }    

    return 0;
}