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
PCB* schedule(SchedAlgo algo, PCB **ready, int quantum);
PCB* fifo_sch(PCB **ready);


//falta sjf, srt e rr

#endif
