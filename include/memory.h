#ifndef MEMORY_H
#define MEMORY_H

#include "bcp.h"

#define TOTAL_PAGES 1024
#define PAGES_OS 64
#define FREE_PAGES (TOTAL_PAGES - PAGES_OS)

typedef struct {
    int busy_page[TOTAL_PAGES];
    int owner_page[TOTAL_PAGES]; // id do processo dono da página
    int reference[TOTAL_PAGES]; // bit de segunda chance
} mem_manager;

void mem_init(mem_manager *mem);
int alocate_pages(mem_manager *mem, BCP *proc, int paginas);
void free_pages(mem_manager *mem, BCP *proc);
void swap_pages(mem_manager *mem, BCP *proc, int pagina_logica);

#endif
