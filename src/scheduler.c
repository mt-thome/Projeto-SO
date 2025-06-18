#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/scheduler.h"
#include "../include/bcp.h"
#include "../include/memory.h"

void colocar_processo_pilha (BCP **head, BCP *processo){
    processo->state = READY;
    while((*head)->next != NULL){
        head = &(*head)->next;
    }
    (*head)->next = processo;
    processo->next = NULL;
}

BCP *retirar_processo(BCP *head) {
    if (head == NULL) return NULL;
    BCP *best = NULL;
    BCP *prev = NULL;
    BCP *curr = head;
    BCP *prev_best = NULL;
    while (curr != NULL) {
        if (curr->state == READY) {
            if (best == NULL || curr->rw_count < best->rw_count) {
                best = curr;
                prev_best = prev;
            }
        }
        prev = curr;
        curr = curr->next;
    }

    if (best == NULL) return NULL; // Nenhum processo READY encontrado
    end_process(best); // Desconecta o nó retornado da lista
    return best;
}

void inicializar_processos_ready(BCP *head, int tempo_exec) {
    BCP *curr = head;
    while (curr != NULL) {
        curr->state = READY;
        curr = curr->next;
    }   
}
