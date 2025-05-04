#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/bcp.h"
#include "../include/parser.h"

// Cria um programa usando o arquivo de programa sintético
BCP* load_program(const char* file_path, int next_id) {
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
    process->id = get_next_id();
    process->state = READY;
    process->num_io = 0;
    process->num_instr = 0;
    process->num_sem = 0;
    process->pc = 0; 

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

    // Lendo e alocando cada instrução
    while (fgets(line, sizeof(line), file) && process->num_instr < MAX_INSTR) {
        // Aloca uma nova instrução
        instr* inst = (instr*)malloc(sizeof(instr));
        if (!inst) {
            perror("Erro ao alocar instrução");
            // Limpa memória alocada até agora
            for (int i = 0; i < process->num_instr; i++) {
                free(process->instruction[i]);
            }
            free(process);
            fclose(file);
            return NULL;
        }
        
        // Inicializa a instrução
        inst->parameter = 0;
        strcpy(inst->sem, "");
        strcpy(inst->type, "");

        char type[16], arg[16];
        int read = sscanf(line, "%s %s", type, arg);

        if (read >= 1) {
            strcpy(inst->type, type);
            
            if (read == 2) {
                if (type[0] == 'P' || type[0] == 'V') {
                    // Para operações de semáforo
                    strncpy(inst->sem, arg + 1, 7);  // Removendo o $ inicial
                } else {
                    // Para outras operações com parâmetro numérico
                    inst->parameter = atoi(arg);
                }
            }
        }

        // Armazena a instrução alocada no array
        process->instruction[process->num_instr] = inst;
        process->num_instr++;
    }

    fclose(file);
    return process;
}
