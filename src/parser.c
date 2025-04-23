#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/bcp.h"
#include "../include/parser.h"

BCP* carregar_programa(const char* caminho_arquivo, int id_processo) {
    FILE* arquivo = fopen(caminho_arquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo do programa sintético");
        return NULL;
    }

    BCP* processo = malloc(sizeof(BCP));
    if (!processo) {
        perror("Erro ao alocar BCP");
        fclose(arquivo);
        return NULL;
    }

    processo->id = id_processo;
    processo->pc = 0;
    processo->state = READY;
    processo->num_io = 0;
    processo->num_instr = 0;
    processo->num_sem = 0;

    char linha[128];
    fgets(processo->name, MAX_NAME, arquivo);
    processo->name[strcspn(processo->name, "\n")] = 0;

    fgets(linha, sizeof(linha), arquivo);
    processo->seg_id = atoi(linha);

    fgets(linha, sizeof(linha), arquivo);
    processo->priority = atoi(linha);

    fgets(linha, sizeof(linha), arquivo);
    processo->seg_size = atoi(linha);

    fgets(linha, sizeof(linha), arquivo);
    char* token = strtok(linha, " \n");
    while (token && processo->num_sem < MAX_SEM) {
        strncpy(processo->semaforos[processo->num_sem++], token, 7);
        token = strtok(NULL, " \n");
    }

    while (fgets(linha, sizeof(linha), arquivo)) {
        instr* inst = &processo->instruction[processo->num_instr];
        inst->parametro = 0;
        strcpy(inst->semaforo, "");

        char tipo[16], arg[16];
        int lidos = sscanf(linha, "%s %s", tipo, arg);

        if (lidos == 1) {
            strcpy(inst->tipo, tipo);
        } else if (lidos == 2) {
            strcpy(inst->tipo, tipo);
            if (tipo[0] == 'P' || tipo[0] == 'V') {
                strncpy(inst->semaforo, arg + 1, 7); // remove parênteses
            } else {
                inst->parametro = atoi(arg);
            }
        }

        processo->num_instr++;
        if (processo->num_instr >= MAX_INSTR) break;
    }

    fclose(arquivo);
    return processo;
}
