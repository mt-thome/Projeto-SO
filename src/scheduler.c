#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/scheduler.h"
#include "../include/events.h"
#include "../include/bcp.h"




void colocar_processo_pilha (BCP **head, BCP *processo){

    processo->state = READY;

    while(*head->next!=NULL){

        *head = *head->next;

    }

    *head->next = processo;
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
            if (best == NULL || curr->rw_count > best->rw_count) {
                best = curr;
                prev_best = prev;
            }
        }
        prev = curr;
        curr = curr->next;
    }

    if (best == NULL) return NULL; // Nenhum processo READY encontrado

    // Remove o melhor processo da lista
    if (prev_best == NULL) {
        // O melhor processo está no início da lista
        head = best->next;
    } else {
        // O melhor processo está no meio ou fim da lista
        prev_best->next = best->next;
    }

    best->next = NULL; // Desconecta o nó retornado da lista
    return best;
}

void inicializar_processos_ready(BCP *head) {
    BCP *curr = head;
    while (curr != NULL) {
        curr->state = READY;
        curr = curr->next;
    }
}



// int num_processos = 0;
// #define time_slicing

// BCP** init (BCP *BCP_lista, int *num_processos){

//     //Percorrer a lista e transformar em array estático
//     BCP *head = BCP_lista;
//     *num_processos = 0;
//     while(head!=NULL){

//         (*num_processos)++;
//         head = head->next;

//     }

//     BCP **Lista = malloc(*num_processos * sizeof(BCP*));

//     head = BCP_lista;

//     for (int i=0;i<*num_processos;i++){
//         Lista[i] = head;
//         head = head->next;
//     }


//     return Lista;
// }



// BCP *Array_To_List(BCP **Array, int *num_processos){

    


//     for (int i = 0; i < *num_processos - 1; i++) {
//             Array[i]->next = Array[i + 1];
//         }
//         Array[*num_processos - 1]->next = NULL;  // último elemento aponta para NULL
//         return Array[0]; // novo head da lista
    
// }

// BCP *rodar_Processo(BCP *processo){
//     int i=0;
//     for(i=0; i<processo->tempo_exec&&i<time_slicing;i++);
//     //ao sair do for, verificar se foi por time_slicing ou tempo exec acabou
//     if(i<tempo_exec){
//         processo->tempo_exec = processo->tempo_exec - i;
//         return processo;
//     }
//     else{
//         //Rodou tudo
//         //...
//     }
// }