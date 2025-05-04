#ifndef BCP_H
#define BCP_H

#define MAX_SEM 10
#define MAX_INSTR 100
#define MAX_NAME 50
#define MAX_PAGINAS 16

#include "parser.h"
#include "memory.h"

typedef enum {
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} p_state;

typedef struct BCP {  // Adicionado nome da struct para auto-referência
    int id;
    char name[MAX_NAME];
    int priority;
    int rw_count; // Contador de operações de leitura/escrita
    int seg_id;
    int seg_size; // em KB
    int allocated_pages[MAX_PAGINAS]; // índices das páginas na memória
    int num_pages; // quantidade de páginas alocadas
    p_state state;
    char semaforos[MAX_SEM][MAX_NAME]; // nomes dos semáforos usados
    int num_sem;
    instr *instruction[MAX_INSTR]; // ponteiros para as instruções do programa
    int num_instr;
    int quantum_time; // tempo de execução do processo
    struct BCP *next; // ponteiro para o próximo processo (corrigido de 'BCP *' para 'struct BCP *')
    
    // Controle de recursos
    int disco_em_uso;
    int impressora_em_uso;
} BCP;

// Protótipos de funções atualizados
void init_bcp(); // Inicializa estruturas do sistema de BCPs
BCP *new_process(const char *file, BCP *new); // Adiciona parâmetro next
void end_process(BCP *proc); 
BCP *get_process(int id); // Busca processo por ID
BCP *get_bcp(); // Retorna a lista de processos

#endif