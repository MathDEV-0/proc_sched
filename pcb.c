#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"

const char* states[] = {
    "created",
    "ready",
    "executing",
    "finished",
    "blocked",
    "suspended, blocked",
    "suspended, ready"
};

PCB* pcb_new() {
    PCB* p = malloc(sizeof(PCB));
    p->id = -1;
    p->state = CREATE;
    p->next = NULL;
    p->remaining_time = 0;
    p->block_moment = -1;
    p->block_time = 0;
    return p;
}

void pcb_push(PCB **queue, PCB *item) {
    if(item == NULL) return;
    item->next = NULL;
    if(*queue == NULL) {
        *queue = item;
    } else {
        PCB *tmp = *queue;
        while(tmp->next != NULL) tmp = tmp->next;
        tmp->next = item;
    }
}

PCB* pcb_pop(PCB **queue) {
    if(*queue != NULL) {
        PCB* aux = *queue;
        *queue = aux->next;
        aux->next = NULL;
        return aux;
    }
    return NULL;
}

void log_state(int t, PCB *p) {
    printf("%02d:P%d -> %s (%d)\n", t, p->id+1, states[p->state], p->remaining_time);
}
