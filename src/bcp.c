#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/bcp.h"
#include "../include/memory.h"
#include "../include/parser.h"

static BCP* bcp_list;
static int next_id;

void init_bcp() {
    bcp_list = NULL;
    next_id = 1;
}

BCP *new_process(const char *file_path) {
    // Carregar processo do arquivo sintético usando o parser
    BCP *new_proc = load_program(file_path, next_id++);
    
    if (!new_proc) {
        fprintf(stderr, "Erro ao carregar programa do arquivo: %s\n", file_path);
        return NULL;
    }
    
    // Inicializa as páginas alocadas
    // Falta implementar a alocação de memória, beijo santias
    for (int i = 0; i < MAX_PAGINAS; i++) {
        new_proc->allocated_pages[i] = -1;
    }

    // Adiciona o novo processo à lista de processos
    if (bcp_list == NULL) {
        bcp_list = new_proc;
        new_proc->next = NULL;
    } else {
        BCP *temp = bcp_list;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_proc;
        new_proc->next = NULL;
    }

    printf("Novo processo criado com ID %d a partir do arquivo: %s\n", new_proc->id, file_path);
    return new_proc;
}