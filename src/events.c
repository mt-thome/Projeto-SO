#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/events.h"

Semaphore semaphores[MAX_SEM];
int num_semaphores = 0;

Semaphore* find_semaphore(const char* name) {
    for (int i = 0; i < num_semaphores; i++) {
        if (strcmp(semaphores[i].name, name) == 0) {
            return &semaphores[i];
        }
    }
    return NULL;
}

int create_semaphore(const char* name, int initial_value) {
    if (find_semaphore(name)) {
        return -1; // Semáforo já existe
    }
    if (num_semaphores >= MAX_SEM) {
        return -2; // Limite de semáforos atingido
    }
    strncpy(semaphores[num_semaphores].name, name, MAX_NAME);
    semaphores[num_semaphores].value = initial_value;
    semaphores[num_semaphores].waiting_processes = NULL; // Inicializa lista de processos esperando
    semaphores[num_semaphores].waiting_count = 0; //
    num_semaphores++;
    return 0; // Sucesso
}

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
            new_process(proc);
            return 0;
            break;
        
        case PROCESS_FINISH:
            proc->state = TERMINATED;
            printf("[PROCESSO] Processo %d finalizado\n", proc->id);
            end_process(proc);
            return 0;
            break;
            
        case SEMAPHORE_P:{
            Semaphore* sem = find_semaphore(arg);
            if (sem) {
                if (sem->value > 0) {
                    sem->value--;
                    printf("[SEMÁFORO] Processo %d adquiriu %s (contagem: %d)\n", proc->id, arg, sem->value);
                    proc->state = RUNNING;
                    if(sem->waiting_processes == NULL) {
                        sem->waiting_processes = malloc(sizeof(int) * MAX_SEM);
                    }
                    sem->waiting_processes->id_process = proc->id;
                    set_cpu_pc(proc->instruction[instr_index]->quantum_time);
                } else {
                    proc->state = BLOCKED;
                    sem->waiting_count++;
                    sem->waiting_processes->id_process = proc->id;
                    printf("[SEMÁFORO] Processo %d bloqueado em %s (indisponível)\n", proc->id, arg);
                    
                }
            } else {
                // Cria semáforo se não existir com valor inicial 1
                if (create_semaphore(arg, 1) == 0) {
                    sem = find_semaphore(arg);
                    sem->value--;
                    sem->waiting_count = 0;
                    sem->waiting_processes = malloc(sizeof(int) * MAX_SEM);
                    sem->waiting_processes->id_process = proc->id;
                    printf("[SEMÁFORO] Criado novo semáforo %s para processo %d\n", arg, proc->id);
                } else {
                    fprintf(stderr, "[ERRO] Semáforo %s não encontrado\n", arg);
                    return -1;
                }
            }
            return 0;
            break;
        }
        
        case SEMAPHORE_V:{
            Semaphore* sem = find_semaphore(arg);
            if (sem) {
                sem->value++;
                sem->waiting_count--;
                // Remove o processo da lista de processos esperando
                if (sem->waiting_processes != NULL) {
                    process_node *proc_node = sem->waiting_processes;
                    process_node *prev = NULL;
                    while(proc_node!= NULL) { {
                        if (proc_node->id_process == proc->id) {
                            if(prev == NULL) {
                                sem->waiting_processes = proc_node->next; 
                            } else {
                                prev->next = proc_node->next; 
                            }
                            sem->waiting_count--;
                            break;
                        }
                    prev = proc_node;
                    proc_node = proc_node->next;
                    }
                }
                sem->waiting_processes = realloc(sem->waiting_processes, sizeof(int) * sem->waiting_count); 
                printf("[SEMÁFORO] Processo %d liberou %s (contagem: %d)\n", proc->id, arg, sem->value);
                if(sem->waiting_count > 0) {
                    // Acorda um processo bloqueado, se houver
                    int pid = sem->waiting_processes->id_process;
                    BCP* waiting_proc = get_bcp();
                    while (waiting_proc && waiting_proc->id != pid) {
                        waiting_proc = waiting_proc->next;
                    }
                    if (waiting_proc) {
                        Semaphore *bcp = find_semaphore("BCP_Semaphore");
                        if(bcp->waiting_count> 0){
                            waiting_proc->state = READY;
                        }
                        printf("[SEMÁFORO] Processo %d acordado por %s\n", waiting_proc->id, arg);
                    }
                }
                else {
                    printf("[SEMÁFORO] Nenhum processo bloqueado em %s\n", arg);
                }
                sem->waiting_processes = realloc(sem->waiting_processes, sizeof(int) * (sem->waiting_count - 1));
            } else {
                fprintf(stderr, "[ERRO] Semáforo %s não encontrado\n", arg);
                return -1;
            }
            return 0;
            break;
        }
        
        case MEM_LOAD_REQ:{
            proc->state = BLOCKED;
            int tempo_carregamento = proc->instruction[proc->instr_index]->quantum_time; 
            printf("[MEMÓRIA] Processo %d solicitou carregamento (%d unidades)\n", proc->id, tempo_carregamento);
            inicializar_processos_ready(get_bcp(), proc->instruction[proc->instr_index]->quantum_time);
            // Aqui você pode adicionar lógica para alocar memória
            return 0;
            break;
        }
            
        case DISK_REQUEST:{
            Semaphore *sem = find_semaphore("IO");
            if (!sem) {
                fprintf(stderr, "[ERRO] Semáforo IO não encontrado\n");
                return -1;
            }
            
            if (sem->value <= 0) {
                // Recurso indisponível - adicionar à lista ordenada
                printf("[DISCO] Processo %d bloqueado aguardando E/S de disco\n", proc->id);
                
                process_node *new_node = malloc(sizeof(process_node));
                if (!new_node) {
                    fprintf(stderr, "[ERRO] Falha ao alocar memória para processo na fila\n");
                    return -1;
                }
                
                new_node->id_process = proc->id;
                new_node->quantum_time = proc->instruction[proc->instr_index]->quantum_time;
                new_node->num_instr = proc->num_instr;
                new_node->next = NULL;
                
                // Inserção ordenada por quantum_time crescente
                if (sem->waiting_processes == NULL || 
                    sem->waiting_processes->quantum_time > new_node->quantum_time) {
                    // Inserir no início
                    new_node->next = sem->waiting_processes;
                    sem->waiting_processes = new_node;
                } else {
                    // Encontrar posição correta
                    process_node *current = sem->waiting_processes;
                    while (current->next != NULL && 
                           current->next->quantum_time <= new_node->quantum_time) {
                        current = current->next;
                    }
                    new_node->next = current->next;
                    current->next = new_node;
                }
                
                sem->waiting_count++;
                proc->state = BLOCKED;
                
            } else {
                // Recurso disponível - usar diretamente
                sem->value--;
                printf("[DISCO] Processo %d obteve acesso ao disco\n", proc->id);
                proc->disco_em_uso = 1;
                proc->state = BLOCKED; // Ainda fica bloqueado durante a operação
            }
            
            set_cpu_qt(time_slicing - proc->instruction[proc->instr_index]->quantum_time);
            printf("[DISCO] Processo %d solicitou E/S de disco (%d unidades)\n", 
                   proc->id, proc->instruction[proc->instr_index]->quantum_time);
            
            inicializar_processos_ready(get_bcp(), proc->instruction[proc->instr_index]->quantum_time);
            proc->instr_index++;
            
            return 0;
            break;
        }
            
        case PRINT_REQUEST:{
            proc->state = BLOCKED;
            set_cpu_qt(time_slicing-proc->instruction[proc->instr_index]->parameter);
            printf("[DISCO] Processo %d solicitou E/S de disco (%d unidades)\n", proc->id, proc->instruction[proc->instr_index]->parameter);
            inicializar_processos_ready(get_bcp(), proc->instruction[proc->instr_index]->parameter);
            return 0;
            break;
        }
            
        case FS_REQUEST:{
            proc->state = BLOCKED;
            printf("[ARQUIVOS] Processo %d solicitou operação no sistema de arquivos\n", proc->id);
            return 0;
            break;
        }
            
        case PROCESS_RUN: {
            proc->state = RUNNING;
            int tempo_exec = 0;
            
            // Enquanto ainda houver instruções para executar
            while (proc->instr_index < proc->num_instr) {
                instr* current_instr = proc->instruction[proc->instr_index];
                int qt_instr = current_instr->quantum_time;
                // Verifica se ainda cabe dentro do quantum total
                if (tempo_exec + qt_instr <= time_slicing) {
        
                    // Simula instruções com base no tipo
                    if (strcmp(current_instr->type, "exec") == 0) {
                        printf("[EXEC] Processo %d executando por %d unidades\n", proc->id, qt_instr);
                        usleep(qt_instr * 1000);
                    }
                    else if (strcmp(current_instr->type, "read") == 0 || strcmp(current_instr->type, "write") == 0) {
                        proc->rw_count++;
                        printf("[E/S] Processo %d iniciando %s (%d unidades)\n", proc->id, current_instr->type, qt_instr);
                        sys_call(DISK_REQUEST, proc, NULL, proc->instr_index);
                        usleep(qt_instr * 1000);
                    }
                    else if (strcmp(current_instr->type, "print") == 0) {
                        printf("[PRINT] Processo %d pediu impressão (%d)\n", proc->id, current_instr->parameter);
                        sys_call(PRINT_REQUEST, proc, NULL, proc->instr_index);
                        usleep(current_instr->parameter * 1000);
                    }
                    else if(current_instr->type[0] == 'P'){
                        printf("[SEMÁFORO] Processo %d solicitou P (%s)\n", proc->id, current_instr->sem);
                        sys_call(SEMAPHORE_P, proc, current_instr->sem, proc->instr_index);
                    }
                    else if(current_instr->type[0] == 'V') {
                        printf("[SEMÁFORO] Processo %d liberou V (%s)\n", proc->id, current_instr->sem);
                        sys_call(SEMAPHORE_V, proc, current_instr->sem, proc->instr_index);
                    }
        
                    // Atualiza tempo total e índice da próxima instrução
                    tempo_exec -= qt_instr;
                    proc->instr_index++;
                    executar_processo(proc);
                    return 0;
                }
                else {
                    // Quantum estourou — salva estado atual
                    proc->quantum_time -= time_slicing;
                    proc->instruction[proc->instr_index]->quantum_time-= time_slicing;
                    if (strcmp(current_instr->type, "exec") == 0) {
                        printf("[EXEC] Processo %d executando por %d unidades\n", proc->id, time_slicing);
                        usleep(time_slicing * 1000);
                    }
                    else if (strcmp(current_instr->type, "read") == 0 || strcmp(current_instr->type, "write") == 0) {
                        proc->rw_count++;
                        printf("[E/S] Processo %d iniciando %s (%d unidades)\n", proc->id, current_instr->type, time_slicing);
                        sys_call(DISK_REQUEST, proc, NULL, proc->instr_index);
                        usleep(time_slicing * 1000);
                    }
                    else if (strcmp(current_instr->type, "print") == 0) {
                        printf("[PRINT] Processo %d pediu impressão (%d)\n", proc->id, current_instr->parameter);
                        sys_call(PRINT_REQUEST, proc, NULL, proc->instr_index);
                        usleep(current_instr->parameter * 1000);
                    }
                    
                    printf("[QUANTUM] Processo %d atingiu limite de tempo. Restam %d unidades\n", proc->id, proc->quantum_time);
                    printf("[QUANTUM] Processo %d atingiu limite de tempo. Restam %d unidades na instrução %d\n", proc->id, proc->instruction[proc->instr_index]->quantum_time, proc->instr_index);

                    break;
                }
            }
        
            // Se terminou todas as instruções
            if (proc->instr_index >= proc->num_instr) {
                sys_call(PROCESS_FINISH, proc, NULL, 0);
                set_cpu_qt(time_slicing);
            }
        
            break;
        }
        
    default:
        fprintf(stderr, "[ERRO] Chamada de sistema desconhecida: %d\n", e);
        return -1;
    }
    return 0;
    }
}