#include <stdio.h>
#include <stdlib.h>
#include "escalonador.h"

// escolha do algoritmo 
PCB* schedule(SchedAlgo algo, PCB **ready, int quantum) {
    switch(algo) {
        case ALG_FIFO: return fifo_sch(ready);
        default: return NULL;
    }
}


PCB* fifo_sch(PCB **ready) {
    return pcb_pop(ready);
}


