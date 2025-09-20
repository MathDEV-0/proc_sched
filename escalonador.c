#include <stdio.h>
#include <stdlib.h>
#include "escalonador.h"

// escolha do algoritmo 
PCB* schedule(SchedAlgo algo, PCB **ready, int quantum) {
    switch(algo) {
        case ALG_FIFO: return fifo_sch(ready);
        case ALG_SJF:  return sjf_sch(ready);
        default: return NULL;
    }
}


PCB* fifo_sch(PCB **ready) {
    return pcb_pop(ready);
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


