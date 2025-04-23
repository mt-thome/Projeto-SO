#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/bcp.h"
#include "../include/parser.h"

BCP* carregar_programa(const char* file_path, int id_process) {
    FILE* file = fopen(file_path, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo do programa sintético");
        return NULL;
    }

    BCP* process = malloc(sizeof(BCP));
    if (!process) {
        perror("Erro ao alocar BCP");
        fclose(file);
        return NULL;
    }

    process->id = id_process;
    process->pc = 0;
    process->state = READY;
    process->num_io = 0;
    process->num_instr = 0;
    process->num_sem = 0;

    char linha[128];
    fgets(process->name, MAX_NAME, file);
    process->name[strcspn(process->name, "\n")] = 0;

    fgets(linha, sizeof(linha), file);
    process->seg_id = atoi(linha);

    fgets(linha, sizeof(linha), file);
    process->priority = atoi(linha);

    fgets(linha, sizeof(linha), file);
    process->seg_size = atoi(linha);

    fgets(linha, sizeof(linha), file);
    char* token = strtok(linha, " \n");
    while (token && process->num_sem < MAX_SEM) {
        strncpy(process->semaforos[process->num_sem++], token, 7);
        token = strtok(NULL, " \n");
    }

    while (fgets(linha, sizeof(linha), file)) {
        instr* inst = &process->instruction[process->num_instr];
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

        process->num_instr++;
        if (process->num_instr >= MAX_INSTR) break;
    }

    fclose(file);
    return process;
}
