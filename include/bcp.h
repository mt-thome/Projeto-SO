#ifndef BCP_H
#define BCP_H

#define MAX_SEM 10
#define MAX_INSTR 100
#define MAX_NOME 50
#define MAX_PAGINAS 16

typedef enum {
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} p_state;

typedef struct {
    int id;
    char name[MAX_NOME];
    int priority;
    int seg_id;
    int seg_size; // em KB
    int allocated_pages[MAX_PAGINAS]; // índices das páginas na memória
    int num_pages; // quantidade de páginas alocadas
    int num_io; // total de operações read/write executadas
    int pc; // índice da próxima instrução a executar
    p_state state;
    char semaforos[MAX_SEM][MAX_NOME]; // nomes dos semáforos usados
    int num_sem;
    char *instruction[MAX_INSTR]; // ponteiros para as instruções do programa
    int num_instr;
} BCP;

// Funções
BCP *new_process(const char *file);
void end_process(BCP *proc);

#endif
// Fim do arquivo bcp.h