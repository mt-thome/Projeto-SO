#ifndef BCP_H
#define BCP_H

#define MAX_SEM 10
#define MAX_INSTR 100
#define MAX_NAME 50
#define MAX_PAGINAS 16

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
    int seg_id;
    int seg_size; // em KB
    int allocated_pages[MAX_PAGINAS]; // índices das páginas na memória
    int num_pages; // quantidade de páginas alocadas
    int num_io; // total de operações read/write executadas
    int pc; // Adicionado program counter que estava faltando
    p_state state;
    char semaforos[MAX_SEM][MAX_NAME]; // nomes dos semáforos usados
    int num_sem;
    char *instruction[MAX_INSTR]; // ponteiros para as instruções do programa
    int num_instr;
    struct BCP *next; // ponteiro para o próximo processo (corrigido de 'BCP *' para 'struct BCP *')
    
    // Campos de tempo adicionados conforme necessidade
    int tempo_execucao;    // Para controle de 'exec t'
    int tempo_io;         // Para operações de E/S
    int tempo_carregamento; // Para memLoadReq/Finish
    
    // Controle de recursos
    int disco_em_uso;
    int impressora_em_uso;
} BCP;

// Protótipos de funções atualizados
void init_bcp(); // Inicializa estruturas do sistema de BCPs
BCP *new_process(const char *file, BCP *new); // Adiciona parâmetro next
void end_process(BCP *proc); 
BCP *get_process(int id); // Busca processo por ID

#endif
// Fim do arquivo bcp.h