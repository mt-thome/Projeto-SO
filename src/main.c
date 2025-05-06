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
    
    // Inicializa os subsistemas
    mem_init(get_memory_manager());
    init_bcp();
    init_cpu();

    new_process("resources/synthetic_2.txt", NULL);
    
    // Mostra a interface
    show_menu();
        
    return 0;
}