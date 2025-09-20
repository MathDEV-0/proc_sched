#include <stdio.h>
#include <stdlib.h>
#include "escalonador.h"

// escolha do algoritmo 
PCB* schedule(SchedAlgo algo, PCB **ready, int quantum,int t) {
    switch(algo) {
        case ALG_FIFO: return fifo_sch(ready);
        case ALG_SJF:  return sjf_sch(ready);
        case ALG_RR:   return rr_sch(ready, quantum,t);
        default: return NULL;
    }
}


PCB* fifo_sch(PCB **ready) {
    return pcb_pop(ready); //retorna o primeiro processo da fila
}

PCB* sjf_sch(PCB **ready){
    if(*ready == NULL) return NULL;
    PCB *prev = NULL;
    PCB *curr = *ready;
    PCB *shortest = curr;
    PCB *shortest_prev = NULL;

    while(curr != NULL) {
        if(curr->remaining_time < shortest->remaining_time) {
            shortest = curr;
            shortest_prev = prev;
        }
        prev = curr;
        curr = curr->next;
    }

    if(shortest_prev == NULL) {
        *ready = shortest->next;
    } else {
        shortest_prev->next = shortest->next;
    }

    shortest->next = NULL; 

    return shortest;
}

PCB* rr_sch(PCB **ready, int quantum,int t) {
    if(*ready == NULL) return NULL;  //sem processo retorna NULL
    PCB *p = pcb_pop(ready); //pega o primeiro processo da fila
    if(p->remaining_time > quantum) { 
        p->remaining_time -= quantum; //calcula o tempo restante
        pcb_push(ready, p,t); //coloca o processo no final da fila
        return NULL; 
    } else {
        return p;
    }
}
