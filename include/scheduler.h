#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../include/events.h"
#include "../include/bcp.h"

void colocar_processo_pilha (BCP **head, BCP *processo);
BCP *retirar_processo(BCP *head);
void inicializar_processos_ready(BCP *head);
BCP *schedule_next(BCP **queue, int total);

#endif