#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    process->num_instr = 0;
    process->num_sem = 0;
    process->rw_count = 0;
    process->quantum_time = 0;
    process->instr_index = 0;
    process->next = NULL; 

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
        
        inst->pc = get_next_pc(); 
        inst->parameter = 0;
        strcpy(inst->sem, "");
        strcpy(inst->type, "");

        char type[16], arg[16];
        int read = sscanf(line, "%s %s", type, arg);

        if (read >= 1) {
            strcpy(inst->type, type);
            if(strcmp(type, "read") == 0 || strcmp(type, "write") == 0){
                process->quantum_time += BASE_DISK_TIME + atoi(arg) * SEEK_TIME_PER_TRACK;
                process->instruction[process->num_instr]->quantum_time = BASE_DISK_TIME + atoi(arg) * SEEK_TIME_PER_TRACK;
            }
            if (read == 2) {
                if (type[0] == 'P') {
                    strncpy(inst->sem, arg + 1, 7);  
                    sys_call(SEMAPHORE_P, process, inst->sem, 0);  
                } else if(type[0] == 'V'){
                    strncpy(inst->sem, arg + 1, 7);  
                    sys_call(SEMAPHORE_V, process, inst->sem, 0);
                } else {
                    inst->parameter = atoi(arg);
                }
            }
        }
        // Armazena a instrução alocada no array
        process->instruction[process->num_instr] = inst;
        process->num_instr++;
    }
    process->num_instr--; // Decrementa para não contar a última linha lida
    for( int i = 0; i < process->num_instr; i++) {
        if(strcmp(process->instruction[i]->type, "exec") == 0 || strcmp(process->instruction[i]->type, "print") == 0)
            process->quantum_time += process->instruction[i]->parameter;
    }
    set_cpu_pc((get_cpu()).pc+process->num_instr); 
    fclose(file);
    return process;
}

