#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/interface.h"

int show_menu() {
    int option = 0;
    printf("\nUsmininOs\n\n");
    printf("Escolha uma opção:\n");
    printf("1. Criar novo processo\n");
    printf("2. Listar processos\n");
    printf("3. Listar memória\n");
    printf("4. Sair\n");
    printf("5. Loop de CPU por um timing slice\n");

    scanf("%d", &option);
    switch (option) {
        case 1:
            char name[MAX_NAME];
            char line[100];
            printf("Digite o caminho do arquivo do programa sintético: ");
            scanf("%s", name);
            new_process(name); 
            break;
        case 2:
            show_processes(); 
            break;
        case 3:
            show_mem(); 
            break;
        case 4:
            printf("Saindo...\n");
            return 1; // Sair do loop
        case 5:
            loop_cpu();
            break;
        default:
            printf("Opção inválida. Tente novamente.\n");
    }
    show_menu();
    return 0; // Continuar no loop
}

// Função para mostrar os processos no sistema
void show_processes() {
    BCP *current = get_bcp(); 
    int count = 0;

    printf("\n===== LISTA DE PROCESSOS =====\n");
    if (!current) {
        printf("Não há processos no sistema.\n");
        return;
    }

    // printf("%-5s %-20s %-10s %-10s %-10s %-10s\n", "ID", "Nome", "Estado", "Prioridade", "Segmento", "Páginas");
    while (current) {
        // Converte o estado do processo para texto
        char *estado;
        switch (current->state) {
            case READY: estado = "Pronto"; break;
            case RUNNING: estado = "Executando"; break;
            case BLOCKED: estado = "Bloqueado"; break;
            case TERMINATED: estado = "Terminado"; break;
            default: estado = "Desconhecido";
        }
        
        printf("ID: %d\n", current->id);
        printf("Nome: %s\n", current->name);
        printf("Estado: %s\n", estado);
        printf("Prioridade: %d\n", current->priority);
        printf("Segmento: %d\n", current->seg_id);
        printf("Páginas: %d\n", current->num_pages);
        
        //printf("%-5d %-20s %-10s %-10d %-10d %-10d", current->id, current->name, estado, current->priority, current->seg_id, current->num_pages);
        count++;
        current = current->next;
    }
    printf("\nTotal: %d processos\n", count);
}

// Função para mostrar o estado da memória
void show_mem() {
    mem_manager *mem = get_memory_manager();
    int total_pages = TOTAL_PAGES;
    int system_pages = PAGES_OS;
    int user_pages = total_pages - system_pages;
    int used_pages = 0;
    int free_pages = 0;
    
    printf("\n===== ESTADO DA MEMÓRIA =====\n");
    
    // Conta páginas livres e ocupadas
    for (int i = system_pages; i < total_pages; i++) {
        if (mem->busy_page[i]) {
            used_pages++;
        } else {
            free_pages++;
        }
    }
    
    // Exibe estatísticas gerais
    printf("Total de páginas:        %d páginas (1KB cada)\n", total_pages);
    printf("Páginas do sistema:      %d páginas (%d KB)\n", system_pages, system_pages);
    printf("Páginas para usuários:   %d páginas (%d KB)\n", user_pages, user_pages);
    printf("Páginas ocupadas:        %d páginas (%d KB)\n", used_pages, used_pages);
    printf("Páginas livres:          %d páginas (%d KB)\n", free_pages, free_pages);
    printf("Taxa de ocupação:        %.1f%%\n", (float)used_pages / user_pages * 100);
    
    // Exibe mapa de memória
    printf("\n== MAPA DE MEMÓRIA ==\n");
    printf("S = Sistema, L = Livre, número = ID do processo proprietário\n");
    
    for (int i = 0; i < total_pages; i++) {
        if (i < system_pages) {
            printf("S ");
        } else if (!mem->busy_page[i]) {
            printf("L ");
        } else {
            printf("%d ", mem->owner_page[i]);
        }
        
        // Quebra de linha a cada 10 páginas para melhor visualização
        if ((i + 1) % 10 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}