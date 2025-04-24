#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/bcp.h"
#include "../include/memory.h"
#include "../include/parser.h"
#include "../include/interface.h"

void show_menu() {
    int option = 0;
    printf("UsmininOs");
    printf("Escolha uma opção:\n");
    printf("1. Criar novo processo\n");
    printf("2. Listar processos\n");
    printf("3. Listar memória\n");
    printf("4. Sair\n");
    scanf("%d", &option);
    switch (option) {
        case 1:
            BCP *new = malloc(sizeof(BCP));
            printf("Digite o nome do arquivo do programa sintético: ");
            scanf("%s", new->name);
            printf("Digite o ID do processo: ");
            scanf("%d", &new->id);
            printf("Digite o ID do segmento: ");
            scanf("%d", &new->seg_id);
            printf("Digite o tamanho do segmento (em KB): ");
            scanf("%d", &new->seg_size);
            printf("Digite a prioridade do processo: ");
            scanf("%d", &new->priority);
            printf("Digite o número de semáforos: ");
            scanf("%d", &new->num_sem);
            for (int i = 0; i < new->num_sem; i++) {
                printf("Digite o nome do semáforo %d: ", i + 1);
                scanf("%s", new->semaforos[i]);
            }
            printf("Digite o número de instruções: ");
            scanf("%d", &new->num_instr);
            for (int i = 0; i < new->num_instr; i++) {
                printf("Digite a instrução %d: ", i + 1);
                new->instruction[i] = malloc(100 * sizeof(char));
                scanf("%s", new->instruction[i]);
                char *cmd = strtok(new->instruction[i], " ");
                if (strcmp(cmd, "read") == 0|| strcmp(cmd, "write") == 0) {
                    new->num_io++;
                }
            }
            new->state = READY;
            new->num_pages = 0;
            new->next = NULL;
            new_process(NULL, new);
            break;
        case 2:
            void show_process(BCP **fila, int total);
            break;
        case 3:
            void show_mem(mem_manager *mem);
            break;
        case 4:
            printf("Saindo...\n");
            exit(0);
        default:
            printf("Opção inválida. Tente novamente.\n");
    }
}