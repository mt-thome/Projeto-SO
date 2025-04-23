#ifndef INTERFACE_H
#define INTERFACE_H

#include "bcp.h"
#include "memory.h"

void show_menu();
void show_process(BCP **fila, int total);
void show_mem(mem_manager *mem);

#endif
// Fim do arquivo interface.h