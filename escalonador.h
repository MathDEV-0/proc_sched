#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "pcb.h"

// algoritmos de escalonamento
typedef enum {
    ALG_RR, 
    ALG_FIFO,
    ALG_SJF,
    ALG_SRT
} SchedAlgo;

// funções protótipos
PCB* fifo_sch(PCB **ready);
PCB* sjf_sch(PCB **ready);
PCB* rr_sch(PCB **ready, int quantum); 
PCB* srt_sch(PCB **ready);              


#endif
