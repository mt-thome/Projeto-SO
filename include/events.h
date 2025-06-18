#ifndef EVENTS_H
#define EVENTS_H

#include "bcp.h"
#include "parser.h"
#include "cpu.h"
#include "memory.h"
#include "scheduler.h"

#define MAX_SEM 10
#define BASE_DISK_TIME 100
#define SEEK_TIME_PER_TRACK 10
#define PRINT_TIME 300

// Tipos de eventos (como especificado no projeto)
typedef enum {
    PROCESS_INTERRUPT = 1,
    PROCESS_CREATE = 2,
    PROCESS_FINISH = 3,
    DISK_REQUEST = 4,
    DISK_FINISH = 5,
    MEM_LOAD_REQ = 6,
    MEM_LOAD_FINISH = 7,
    FS_REQUEST = 8,
    FS_FINISH = 9,
    SEMAPHORE_P = 10,
    SEMAPHORE_V = 11,
    PROCESS_RUN = 12,
    PRINT_REQUEST = 14,
    PRINT_FINISH = 15
} event;

typedef struct process_node{
    int id_process; // Ponteiro para o processo
    struct process_node *next; // Próximo nó na lista
}
process_node;

typedef struct {
    char name[MAX_NAME];
    int value;
    process_node *waiting_processes; // Lista de processos esperando neste semáforo 
    int waiting_count; // Contador de processos esperando
} Semaphore;

Semaphore* find_semaphore(const char* name);
Semaphore* create_semaphore(const char* name, int initial_value);

// Declaração das funções principais
int interrupt_control(event e, BCP* proc, int instr_index);
int sys_call(event e, BCP* proc, const char* arg, int instr_index);

#endif