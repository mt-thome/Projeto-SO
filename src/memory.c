#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/memory.h"

static mem_manager main_memory;
static int memory_initialized = 0;
static int clock_hand = PAGES_OS;

void mem_init(mem_manager *mem) {
    for (int i = 0; i < TOTAL_PAGES; i++) {
        mem->busy_page[i] = (i < PAGES_OS) ? 1 : 0;
        mem->owner_page[i] = -1;
        mem->reference[i] = 0;
    }
}

int allocate_pages(mem_manager *mem, BCP *proc, int paginas) {
    int allocated = 0;

    for (int i = PAGES_OS; i < TOTAL_PAGES && allocated < paginas; i++) {
        if (!mem->busy_page[i]) {
            mem->busy_page[i] = 1;
            mem->owner_page[i] = proc->id;
            mem->reference[i] = 1;
            proc->allocated_pages[allocated++] = i; // salva na tabela do processo
        }
    }

    // retorna -1 se não conseguiu alocar tudo
    return (allocated == paginas) ? 0 : -1;
}

void free_pages(mem_manager *mem, BCP *proc) {
    for (int i = 0; i < proc->num_pages; i++) {
        int page = proc->allocated_pages[i];
        if (page >= PAGES_OS && page < TOTAL_PAGES) {
            mem->busy_page[page] = 0;
            mem->owner_page[page] = -1;
            mem->reference[page] = 0;
        }
    }
}

void swap_pages(mem_manager *mem, BCP *proc, int logical_page) {
    while (1) {
        if (clock_hand >= TOTAL_PAGES) clock_hand = PAGES_OS;

        if (mem->busy_page[clock_hand]) {
            if (mem->reference[clock_hand]) {
                mem->reference[clock_hand] = 0;
            } else {
                // swap
                int old_owner = mem->owner_page[clock_hand];
                mem->owner_page[clock_hand] = proc->id;
                mem->reference[clock_hand] = 1;
                proc->allocated_pages[logical_page] = clock_hand;
                return;
            }
        }
        clock_hand++;
    }
}

mem_manager* get_memory_manager() {
    if (!memory_initialized) {
        mem_init(&main_memory);
        memory_initialized = 1;
        printf("[MEM] Sistema de gerenciamento de memória inicializado.\n");
    }
    return &main_memory;
}
