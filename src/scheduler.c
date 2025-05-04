#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/scheduler.h"
#include "../include/events.h"
#include "../include/bcp.h"

int num_processos = 0;
#define time_slicing

BCP** init (BCP *BCP_lista, int *num_processos){

    //Percorrer a lista e transformar em array estático
    BCP *head = BCP_lista;
    *num_processos = 0;
    while(head!=NULL){

        (*num_processos)++;
        head = head->next;

    }

    BCP **Lista = malloc(*num_processos * sizeof(BCP*));

    head = BCP_lista;

    for (int i=0;i<*num_processos;i++){
        Lista[i] = head;
        head = head->next;
    }


    return Lista;
}



BCP *Array_To_List(BCP **Array, int *num_processos){

    


    for (int i = 0; i < *num_processos - 1; i++) {
            Array[i]->next = Array[i + 1];
        }
        Array[*num_processos - 1]->next = NULL;  // último elemento aponta para NULL
        return Array[0]; // novo head da lista
    
}

BCP *rodar_Processo(BCP *processo){
    int i=0;
    for(i=0; i<processo->tempo_execucao && i<time_slicing; i++);
    //ao sair do for, verificar se foi por time_slicing ou tempo exec acabou
    if(i<processo->tempo_execucao){   
        processo->tempo_execucao = processo->tempo_execucao - i;
        return processo;
    }
    else{
        //Rodou tudo
        //...
    }
}