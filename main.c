#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcb.h"
#include "escalonador.h"

// global queues
PCB *_create, *_ready, *_running, *_finish, *_blocked;
int _memSize, _quantum, _nprocs, _time, _pcount;

// standard input if none provided
const char *default_input = "2 0|3|0 5|0 3|0 4";

int main(int argc, char *argv[]) {
    const char *input = (argc > 1) ? argv[1] : default_input;

    char *buffer = malloc(strlen(input)+1);
    strcpy(buffer, input);

    // melhora escrita do input
    for(int i=0;i<(int)strlen(buffer);i++) if(buffer[i]=='|') buffer[i]='\n';

    char line[256];
    char *buffp = buffer;
    int count;

    // lê memória e quantum (ignora quantum, não desenvolvemos nada além de FIFO)
    sscanf(buffp, "%[^\n]%n", line, &count);
    buffp += count+1;
    sscanf(line, "%d %d", &_memSize, &_quantum);

    // lê número de processos
    sscanf(buffp, "%[^\n]%n", line, &count);
    buffp += count+1;
    sscanf(line, "%d", &_nprocs);

    // inicialização
    _create=_ready=_running=_finish=_blocked=NULL;
    _time=0; _pcount=0;
    int nFinished=0;

    // lê primeiro processo
    sscanf(buffp, "%[^\n]%n", line, &count);
    buffp += count+1;
    int nextT=-1;
    sscanf(line, "%d", &nextT);

    while(nFinished<_nprocs) {
        // criação
        while(nextT>-1 && _time==nextT) {
            PCB *p = pcb_new();
            int params = sscanf(line,"%d %d %db%d",&nextT,&p->remaining_time,&p->block_moment,&p->block_time);
            if(params<4) p->block_moment=-1;
            p->id=_pcount++;
            log_state(_time,p);
            pcb_push(&_create,p);

            if(sscanf(buffp,"%[^\n]%n",line,&count)>=1) {
                buffp+=count+1;
                sscanf(line,"%d",&nextT);
            } else nextT=-1;
        }

        // create -> ready
        while(1) {
            PCB *p=pcb_pop(&_create); if(!p) break;
            p->state=READY;
            pcb_push(&_ready,p);
            log_state(_time,p);
        }

        // despacha se não há running
        if(_running==NULL && _ready!=NULL) {
            PCB *p=schedule(ALG_FIFO,&_ready,_quantum);
            if(p) {
                p->state=RUN;
                pcb_push(&_running,p);
                log_state(_time,p);
            }
        }

        // executa 1 ciclo
        if(_running!=NULL) {
        PCB *p=pcb_pop(&_running);
        p->remaining_time--;

        // se chegou no momento de bloqueio
        if(p->block_moment>=0 && p->remaining_time>0 && p->block_moment==(_time+1)) {
            p->state=BLOCK;
            pcb_push(&_blocked,p);
            log_state(_time+1,p);
        }
        else if(p->remaining_time<=0) {
            p->remaining_time = 0;   // <-- garante que não fica negativo
            p->state=FINISH;
            log_state(_time+1,p);
            pcb_push(&_finish,p);
        }
        else {
            p->state=READY;
            pcb_push(&_ready,p);
            log_state(_time+1,p);
            }
        }

        // finaliza
        while(1) {
            PCB *p=pcb_pop(&_finish); if(!p) break;
            free(p); nFinished++;
        }

        _time++;
    }

    free(buffer);
    return 0;
}

//gcc -std=c11 -O2 -o simso main.c pcb.c escalonador.c