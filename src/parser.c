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

    char line[128];
    fgets(process->name, MAX_NAME, file);
    process->name[strcspn(process->name, "\n")] = 0;

    fgets(line, sizeof(line), file);
    process->seg_id = atoi(line);

    fgets(line, sizeof(line), file);
    process->priority = atoi(line);

    fgets(line, sizeof(line), file);
    process->seg_size = atoi(line);

    fgets(line, sizeof(line), file);
    char* token = strtok(line, " \n");
    while (token && process->num_sem < MAX_SEM) {
        strncpy(process->semaforos[process->num_sem++], token, 7);
        token = strtok(NULL, " \n");
    }

    while (fgets(line, sizeof(line), file)) {
        instr* inst = &process->instruction[process->num_instr];
        inst->parameter = 0;
        strcpy(inst->sem, "");

        char type[16], arg[16];
        int read = sscanf(line, "%s %s", type, arg);

        if (read == 1) {
            strcpy(inst->type, type);
        } else if (read == 2) {
            strcpy(inst->type, type);
            if (type[0] == 'P' || type[0] == 'V') {
                strncpy(inst->sem, arg + 1, 7); // remove parênteses
            } else {
                inst->parameter = atoi(arg);
            }
        }

        process->num_instr++;
        if (process->num_instr >= MAX_INSTR) break;
    }

    fclose(file);
    return process;
}
