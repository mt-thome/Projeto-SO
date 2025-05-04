#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/events.h"

// Variáveis globais para semáforos
typedef struct {
    char name[MAX_NAME];
    int value;
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

int interrupt_control(event e, BCP* proc) {
    switch (e) {
        case PROCESS_INTERRUPT:
            if (proc->state == RUNNING) {
                proc->state = READY;
                printf("[INTERRUPÇÃO] Processo %d: quantum expirado\n", proc->id);
            }
            break;
            
        case DISK_FINISH:
            proc->disco_em_uso = 0;
            proc->state = READY;
            proc->tempo_io = 0;
            printf("[DISCO] Processo %d: operação de disco concluída\n", proc->id);
            break;
            
        case PRINT_FINISH:
            proc->impressora_em_uso = 0;
            proc->state = READY;
            proc->tempo_io = 0;
            printf("[IMPRESSORA] Processo %d: impressão concluída\n", proc->id);
            break;
            
        case MEM_LOAD_FINISH:
            proc->state = READY;
            proc->tempo_carregamento = 0;
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

int sys_call(event e, BCP* proc, const char* arg) {
    switch (e) {
        case PROCESS_CREATE: {
            BCP* new_proc = new_process(arg, NULL);
            if (new_proc) {
                new_proc->state = READY;
                printf("[PROCESSO] Criado processo %d a partir de %s\n", new_proc->id, arg);
                
                // Inicializa campos de tempo
                new_proc->tempo_execucao = 0;
                new_proc->tempo_io = 0;
                new_proc->tempo_carregamento = 0;
                new_proc->disco_em_uso = 0;
                new_proc->impressora_em_uso = 0;
                
                return new_proc->id;
            }
            fprintf(stderr, "[ERRO] Falha ao criar processo a partir de %s\n", arg);
            return -1;
        }
        
        case PROCESS_FINISH:
            proc->state = TERMINATED;
            printf("[PROCESSO] Processo %d finalizado\n", proc->id);
            end_process(proc);
            break;
            
        case SEMAPHORE_P: {
            Semaphore* sem = find_semaphore(arg);
            if (sem) {
                if (sem->value > 0) {
                    sem->value--;
                    printf("[SEMÁFORO] Processo %d adquiriu %s (contagem: %d)\n", 
                           proc->id, arg, sem->value);
                } else {
                    proc->state = BLOCKED;
                    printf("[SEMÁFORO] Processo %d bloqueado em %s (indisponível)\n", 
                           proc->id, arg);
                }
            } else {
                // Cria semáforo se não existir com valor inicial 1
                if (create_semaphore(arg, 1) == 0) {
                    sem = find_semaphore(arg);
                    sem->value--;
                    printf("[SEMÁFORO] Criado novo semáforo %s para processo %d\n",
                           arg, proc->id);
                } else {
                    fprintf(stderr, "[ERRO] Semáforo %s não encontrado\n", arg);
                    return -1;
                }
            }
            break;
        }
        
        case SEMAPHORE_V: {
            Semaphore* sem = find_semaphore(arg);
            if (sem) {
                sem->value++;
                printf("[SEMÁFORO] Processo %d liberou %s (contagem: %d)\n",
                       proc->id, arg, sem->value);
                
                // Poderia acordar processos bloqueados aqui
            } else {
                fprintf(stderr, "[ERRO] Semáforo %s não encontrado\n", arg);
                return -1;
            }
            break;
        }
        
        case MEM_LOAD_REQ:
            proc->state = BLOCKED;
            proc->tempo_carregamento = 3; // 3 unidades de tempo
            printf("[MEMÓRIA] Processo %d solicitou carregamento (%d unidades)\n", 
                   proc->id, proc->tempo_carregamento);
            break;
            
        case DISK_REQUEST:
            proc->state = BLOCKED;
            proc->disco_em_uso = 1;
            proc->tempo_io = 4; // 4 unidades de tempo
            printf("[DISCO] Processo %d solicitou E/S de disco (%d unidades)\n",
                   proc->id, proc->tempo_io);
            break;
            
        case PRINT_REQUEST:
            proc->state = BLOCKED;
            proc->impressora_em_uso = 1;
            proc->tempo_io = 2; // 2 unidades de tempo
            printf("[IMPRESSORA] Processo %d solicitou impressão (%d unidades)\n",
                   proc->id, proc->tempo_io);
            break;
            
        case FS_REQUEST:
            proc->state = BLOCKED;
            printf("[ARQUIVOS] Processo %d solicitou operação no sistema de arquivos\n", proc->id);
            break;
            
        // Caso especial para execução de programa (exec t)
        case PROCESS_RUN:
            if (arg) {
                proc->tempo_execucao = atoi(arg);
                proc->state = RUNNING;
                printf("[PROCESSO] Processo %d executando por %d unidades de tempo\n",
                       proc->id, proc->tempo_execucao);
            }
            break;
            
        default:
            fprintf(stderr, "[ERRO] Chamada de sistema desconhecida: %d\n", e);
            return -1;
    }
    return 0;
}

// Função para atualizar tempos (deve ser chamada periodicamente)
void update_timers(BCP* proc) {
    if (proc->state == RUNNING && proc->tempo_execucao > 0) {
        proc->tempo_execucao--;
        if (proc->tempo_execucao == 0) {
            interrupt_control(PROCESS_INTERRUPT, proc);
        }
    }
    
    if (proc->tempo_io > 0) {
        proc->tempo_io--;
        if (proc->tempo_io == 0) {
            if (proc->disco_em_uso) {
                interrupt_control(DISK_FINISH, proc);
            } else if (proc->impressora_em_uso) {
                interrupt_control(PRINT_FINISH, proc);
            }
        }
    }
    
    if (proc->tempo_carregamento > 0) {
        proc->tempo_carregamento--;
        if (proc->tempo_carregamento == 0) {
            interrupt_control(MEM_LOAD_FINISH, proc);
        }
    }
}