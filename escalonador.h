#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#include "pcb.h"

// algoritmos de escalonamento
typedef enum {
    ALG_FIFO,
    ALG_SJF,
    ALG_RR
} SchedAlgo;

// Protótipos das funções
PCB* schedule(SchedAlgo algo, PCB **ready, int quantum, int t);
PCB* fifo_sch(PCB **ready);
PCB* sjf_sch(PCB **ready);
PCB *rr_sch(PCB **ready, int quantum, int t);        


#endif
