#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/bcp.h"
#include "../include/parser.h"
#include "../include/memory.h"

static BCP* bcp_list;
static int next_id;

int get_next_id() {
    return next_id++;
}

void init_bcp() {
    bcp_list = NULL;
    next_id = 0;
}

BCP *new_process(const char *file_path, BCP *new) {
    BCP *new_proc = NULL;
    mem_manager *mem = get_memory_manager();
    // Verifica se o programa a ser inserido esta correto
    if(file_path == NULL && new == NULL)
        fprintf(stderr, "Erro: Nenhum arquivo ou processo fornecido.\n");
    // Se o arquivo for NULL, significa que o processo é inserido pelo usuário
    else if (file_path == NULL)
        new_proc = new;
    // Se o struct for NULL, significa que o processo é inserido lendo o arquivo
    else if (new == NULL) {
        // Carregar processo do arquivo sintético
        new_proc = load_program(file_path, next_id++);
        if (!new_proc) {
            fprintf(stderr, "Erro ao alocar memória para o novo processo.\n");
            return NULL;
        }
        
        
        // Inicializa as páginas alocadas
        for (int i = 0; i < MAX_PAGINAS; i++) {
            new_proc->allocated_pages[i] = -1;
        }

        // Calcula quantas páginas são necessárias
        #define PAGE_SIZE 1  // 1KB por página
        int pages_needed = (new_proc->seg_size + PAGE_SIZE - 1) / PAGE_SIZE; // Arredonda para cima
    
        // Garante que não exceda o máximo de páginas por processo
        if (pages_needed > MAX_PAGINAS) {
            fprintf(stderr, "Aviso: Processo %d requer %d páginas, limitado a %d páginas.\n", new_proc->id, pages_needed, MAX_PAGINAS);
            pages_needed = MAX_PAGINAS;
        }
    
        // Tenta alocar as páginas
        int result = allocate_pages(mem, new_proc, pages_needed);
        if (result == 0) {
            new_proc->num_pages = pages_needed;
            printf("Processo %d: %d páginas de 1KB alocadas com sucesso.\n", new_proc->id, pages_needed);
        } else {
            // Aloca o que foi possível
            int allocated = 0;
            for (int i = 0; i < MAX_PAGINAS; i++) {
                if (new_proc->allocated_pages[i] != -1)
                    allocated++;
            }
    
            printf("Alocação parcial: %d/%d páginas alocadas diretamente.\n", allocated, pages_needed);
    
            // Para as restantes, usa swap
            for (int i = allocated; i < pages_needed && i < MAX_PAGINAS; i++) {
                printf("Aplicando swap para página lógica %d do processo %d\n", i, new_proc->id);
                swap_pages(mem, new_proc, i);
            }
    
            // Atualiza o número de páginas alocadas
            new_proc->num_pages = (pages_needed < MAX_PAGINAS) ? pages_needed : MAX_PAGINAS;
            printf("Processo %d: %d páginas alocadas após swap.\n", 
            new_proc->id, new_proc->num_pages);
        }
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

void end_process(BCP *proc) {
    if (!proc) {
        fprintf(stderr, "Erro: Tentativa de finalizar um processo NULL\n");
        return;
    }
    printf("Finalizando processo %d (%s)...\n", proc->id, proc->name);
    // Libera as páginas de memória alocadas
    mem_manager *mem = get_memory_manager();
    free_pages(mem, proc);
    // Remove o processo da lista encadeada
    if (bcp_list == proc) {
        // Caso especial: removendo o primeiro processo da lista
        bcp_list = proc->next;
    } else {
        // Encontra o processo anterior na lista
        BCP *prev = bcp_list;
        while (prev && prev->next != proc) {
            prev = prev->next;
        }
        if (prev) {
            // Processo encontrado, ajusta os ponteiros
            prev->next = proc->next;
        } else {
            // Processo não está na lista
            fprintf(stderr, "Aviso: Processo %d não encontrado na lista de processos.\n", proc->id);
        }
    }
    
    // Libera a memória das instruções 
    for (int i = 0; i < proc->num_instr; i++) {
        if (proc->instruction[i]) {
            free(proc->instruction[i]);
        }
    }
    
    printf("Processo %d finalizado e recursos liberados.\n", proc->id);
    free(proc);
}

BCP *get_process(int id) { // Busca um processo na lista de processos pelo ID
    BCP *temp = bcp_list;
    
    while (temp) {
        if (temp->id == id) {
            return temp;
        }
        temp = temp->next;
    }
    // Processo não encontrado
    return NULL;
}

BCP *get_bcp() { // Retorna a lista de processos
    return bcp_list;
}