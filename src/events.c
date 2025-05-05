#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/events.h"

// Variáveis globais para semáforos
typedef struct {
    char name[MAX_NAME];
    int value;
    int waiting_processes[MAX_SEM]; // IDs dos processos esperando
} Semaphore;

Semaphore semaphores[MAX_SEM];
int num_semaphores = 0;

// --- Funções auxiliares ---

// Encontra um semáforo pelo nome
Semaphore* find_semaphore(const char* name) {
    for (int i = 0; i < num_semaphores; i++) {
        if (strcmp(semaphores[i].name, name) == 0) {
            return &semaphores[i];
        }
    }
    return NULL;
}

// Cria um novo semáforo se não existir
int create_semaphore(const char* name, int initial_value) {
    if (find_semaphore(name)) {
        return -1; // Semáforo já existe
    }
    if (num_semaphores >= MAX_SEM) {
        return -2; // Limite de semáforos atingido
    }
    
    strncpy(semaphores[num_semaphores].name, name, MAX_NAME);
    semaphores[num_semaphores].value = initial_value;
    num_semaphores++;
    return 0;
}

// --- Implementação completa em português ---

int interrupt_control(event e, BCP* proc, int instr_index) {
    switch (e) {
        case PROCESS_INTERRUPT:
            if (proc->state == RUNNING) {
                proc->state = READY;
                proc->quantum_time = 0; // Reinicia o quantum
                end_process(proc);
                printf("[INTERRUPÇÃO] Processo %d: quantum expirado\n", proc->id);
            }
            break;
            
        case DISK_FINISH:
            proc->disco_em_uso = 0;
            proc->state = READY;
            proc->instruction[instr_index]->quantum_time = 0;
            printf("[DISCO] Processo %d: operação de disco concluída\n", proc->id);
            break;
            
        case PRINT_FINISH:
            proc->impressora_em_uso = 0;
            proc->state = READY;
            proc->instruction[instr_index]->quantum_time = 0;
            printf("[IMPRESSORA] Processo %d: impressão concluída\n", proc->id);
            break;
            
        case MEM_LOAD_FINISH:
            proc->state = READY;
            proc->quantum_time = 0;
            printf("[MEMÓRIA] Processo %d: carregamento concluído\n", proc->id);
            break;
            
        case FS_FINISH:
            proc->state = READY;
            printf("[ARQUIVOS] Processo %d: operação com arquivos concluída\n", proc->id);
            break;
            
        default:
            fprintf(stderr, "[ERRO] Evento de interrupção desconhecido: %d\n", e);
            return -1;
    }
    return 0;
}

int sys_call(event e, BCP* proc, const char* arg, int instr_index) {
    switch (e) {
        case PROCESS_CREATE:
            new_process(NULL, proc);
            break;
        
        case PROCESS_FINISH:
            proc->state = TERMINATED;
            printf("[PROCESSO] Processo %d finalizado\n", proc->id);
            end_process(proc);
            break;
            
        case SEMAPHORE_P:
            Semaphore* sem = find_semaphore(arg);
            if (sem) {
                if (sem->value > 0) {
                    sem->value--;
                    printf("[SEMÁFORO] Processo %d adquiriu %s (contagem: %d)\n", proc->id, arg, sem->value);
                    proc->state = RUNNING;
                    sem->waiting_processes[sem->value] = proc->id;
                } else {
                    proc->state = BLOCKED;
                    printf("[SEMÁFORO] Processo %d bloqueado em %s (indisponível)\n", proc->id, arg);
                    
                }
            } else {
                // Cria semáforo se não existir com valor inicial 1
                if (create_semaphore(arg, 1) == 0) {
                    sem = find_semaphore(arg);
                    sem->value--;
                    sem->waiting_processes[sem->value] = proc->id;
                    printf("[SEMÁFORO] Criado novo semáforo %s para processo %d\n", arg, proc->id);
                } else {
                    fprintf(stderr, "[ERRO] Semáforo %s não encontrado\n", arg);
                    return -1;
                }
            }
            break;
        
        case SEMAPHORE_V:
            Semaphore* sem = find_semaphore(arg);
            if (sem) {
                sem->value++;
                printf("[SEMÁFORO] Processo %d liberou %s (contagem: %d)\n", proc->id, arg, sem->value);
                sem ->waiting_processes[sem->value] = 0; 
                // Poderia acordar processos bloqueados aqui
            } else {
                fprintf(stderr, "[ERRO] Semáforo %s não encontrado\n", arg);
                return -1;
            }
            break;
        
        case MEM_LOAD_REQ:
            proc->state = BLOCKED;
            int tempo_carregamento = proc->instruction[instr_index]->quantum_time; 
            printf("[MEMÓRIA] Processo %d solicitou carregamento (%d unidades)\n", proc->id, tempo_carregamento);
            inicializar_processos_ready(get_bcp(), proc->instruction[instr_index]->quantum_time);
            // Aqui você pode adicionar lógica para alocar memória
            break;
            
        case DISK_REQUEST:
            proc->state = BLOCKED;
            set_cpu_qt(time_slicing-proc->instruction[proc->instr_index]->quantum_time);
            printf("[DISCO] Processo %d solicitou E/S de disco (%d unidades)\n", proc->id, proc->instruction[instr_index]->quantum_time);
            inicializar_processos_ready(get_bcp(), proc->instruction[instr_index]->quantum_time);
            break;
            
        case PRINT_REQUEST:
            proc->state = BLOCKED;
            set_cpu_qt(time_slicing-proc->instruction[proc->instr_index]->parameter);
            printf("[DISCO] Processo %d solicitou E/S de disco (%d unidades)\n", proc->id, proc->instruction[instr_index]->parameter);
            inicializar_processos_ready(get_bcp(), proc->instruction[instr_index]->parameter);
            break;
            
        case FS_REQUEST:
            proc->state = BLOCKED;
            printf("[ARQUIVOS] Processo %d solicitou operação no sistema de arquivos\n", proc->id);
            break;
            
        case PROCESS_RUN:
            proc->state = RUNNING;
            int tempo_exec = 0;
            int i = 0;
            int instr_index = 0;
            if (proc->quantum_time > time_slicing) {
                while(proc->instr_index < proc->num_instr) {
                    if(tempo_exec+proc->instruction[proc->instr_index]->quantum_time < time_slicing) {
                        if(strcmp(proc->instruction[instr_index]->type, "exec" == 0)) {
                            tempo_exec += proc->instruction[instr_index]->quantum_time;
                            instr_index++;
                            usleep(tempo_exec * 1000);    
                        }
                        if(strcmp(proc->instruction[instr_index]->type, "read" == 0)) {
                            proc->rw_count++;
                            sys_call(DISK_REQUEST, proc, NULL, instr_index);
                            tempo_exec += proc->instruction[instr_index]->quantum_time;
                            instr_index++;
                            usleep(tempo_exec * 1000);    
                        }
                         
                    } else {
                        proc->quantum_time -= tempo_exec;
                        proc->instr_index = instr_index;
                        usleep(proc->quantum_time * 1000);
                        break;
                    }
                }
                printf("[PROCESSO] Processo %d executando por %d unidades de tempo\n", proc->id, tempo_exec);     
            }else {
                sys_call(PROCESS_FINISH, proc, NULL, instr_index);
                printf("[PROCESSO] Processo %d finalizado\n", proc->id);
                set_cpu_qt(time_slicing);
            }
        default:
            fprintf(stderr, "[ERRO] Chamada de sistema desconhecida: %d\n", e);
            return -1;
    }
    return 0;
}

//void update_timers(BCP* proc, int instr_index) {
//    if (!proc) {
//        return; // Proteção contra ponteiro nulo
//    }
//    
//    // Caso 1: Processo em execução sem parâmetro específico (atualização geral do quantum)
//    if (proc->state == RUNNING && proc->quantum_time > 0 && instr_index == 0) {
//        proc->quantum_time -= get_cpu().quantum_time;
//        if (proc->quantum_time <= 0) {
//            proc->quantum_time = 0;
//            interrupt_control(PROCESS_INTERRUPT, proc, instr_index);
//        }
//    }
//    
//    // Caso 2: Atualização com índice específico de instrução
//    if (proc->quantum_time > 0 && instr_index != 0) {
//        if (instr_index >= 0 && instr_index < proc->num_instr && proc->instruction[instr_index]) {
//            proc->quantum_time -= get_cpu().quantum_time;
//            
//            if (strcmp(proc->instruction[instr_index]->type, "read") == 0 || strcmp(proc->instruction[instr_index]->type, "write") == 0) {
//                // Reduz o tempo restante da instrução
//                proc->instruction[instr_index]->parameter -= get_cpu().quantum_time;
//                sys_call(DISK_REQUEST, proc, NULL, instr_index);
//                // Se a operação de I/O terminou
//                if (proc->instruction[instr_index]->parameter <= 0) {
//                    proc->instruction[instr_index]->parameter = 0;
//                    // Gera interrupção apropriada quando operação termina
//                    if (strcmp(proc->instruction[instr_index]->type, "read") == 0) {
//                        interrupt_control(DISK_FINISH, proc, instr_index);
//                    } else {
//                        interrupt_control(DISK_FINISH, proc, instr_index);
//                    }
//                }
//            }
//        }
//    }
//    
//    // Atualiza contadores de I/O e carregamento de memória
//    if (proc->state == BLOCKED) {
//        if (proc->instruction[instr_index]->quantum_time > 0) {
//            proc->instruction[instr_index]->quantum_time -= get_cpu().quantum_time;
//            if (proc->instruction[instr_index]->quantum_time <= 0) {
//                if (proc->disco_em_uso) {
//                    interrupt_control(DISK_FINISH, proc, 0);
//                } else if (proc->impressora_em_uso) {
//                    interrupt_control(PRINT_FINISH, proc, 0);
//                }
//            }
//        }
//    }
//}
