#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcb.h"
#include "escalonador.h"

// Variáveis globais
PCB *_create, *_ready, *_running, *_finish, *_blocked;
int _memSize, _quantum, _nprocs, _time, _pcount;
int processors_count;  // Número de processadores
SchedAlgo _algo;


int main(int argc, char *argv[]) {
    
    if (argc > 2) {
    if (strcmp(argv[2], "fifo") == 0) _algo = ALG_FIFO;
    else if (strcmp(argv[2], "sjf") == 0) _algo = ALG_SJF;
    else if (strcmp(argv[2], "rr") == 0) _algo = ALG_RR;
    else {
        printf("Algoritmo inválido! Usando FIFO.\n");
        _algo = ALG_FIFO;
    }
}   
    printf("Executando o algoritmo de escalonamento: %s\n", 
        (_algo == ALG_FIFO) ? "FIFO" : (_algo == ALG_SJF) ? "SJF" : "RR");

    char *input = (argc > 1) ? argv[1] : "2 2|2|3|0 5|1 3|2 4";

    char *buffer = malloc(strlen(input) + 1);
    strcpy(buffer, input);

    for (int i = 0; i < (int)strlen(buffer); i++)
        if (buffer[i] == '|') buffer[i] = '\n';

    char line[256];
    char *buffp = buffer;
    int count;

    // Lê memória e quantum
    sscanf(buffp, "%[^\n]%n", line, &count);
    buffp += count + 1;
    sscanf(line, "%d %d", &_memSize, &_quantum);

    // Lê número de processadores
    sscanf(buffp, "%[^\n]%n", line, &count);
    buffp += count + 1;
    sscanf(line, "%d", &processors_count);

    // Lê número de processos
    sscanf(buffp, "%[^\n]%n", line, &count);
    buffp += count + 1;
    sscanf(line, "%d", &_nprocs);

    // Inicialização
    _create = _ready = _running = _finish = _blocked = NULL;
    _time = 0;
    _pcount = 0;
    int nFinished = 0;

    // Lê primeiro processo
    sscanf(buffp, "%[^\n]%n", line, &count);
    buffp += count + 1;
    int nextT = -1;
    sscanf(line, "%d", &nextT);

    // Array de processadores
    PCB *processors[processors_count];
    memset(processors, 0, sizeof(processors));

    while (nFinished < _nprocs) {
        // Criação de novos processos
        while (nextT > -1 && _time == nextT) {
            PCB *p = pcb_new();
            int params = sscanf(line, "%d %d %db%d", &nextT, &p->remaining_time, &p->block_moment, &p->block_time);
            if (params < 4) p->block_moment = -1;
            p->id = _pcount++;
            log_state(_time, p);
            pcb_push(&_create, p, _time);

            if (sscanf(buffp, "%[^\n]%n", line, &count) >= 1) {
                buffp += count + 1;
                sscanf(line, "%d", &nextT);
            } else {
                nextT = -1;
            }
        }

        // CREATE -> READY
        while (1) {
            PCB *p = pcb_pop(&_create);
            if (!p) break;
            p->state = READY;
            pcb_push(&_ready, p, _time);
            log_state(_time, p);
        }

        // Despachar processos para processadores livres
        for (int i = 0; i < processors_count; i++) {
            if (processors[i] == NULL && _ready != NULL) {
                PCB *p = schedule(_algo, &_ready, _quantum, _time); 
                if (p) {
                    p->state = RUN;
                    processors[i] = p;
                    log_state(_time, p);
                }
            }
        }

        // Executa processos nos processadores
        for (int i = 0; i < processors_count; i++) {
            PCB *p = processors[i];
            if (p) {
                p->remaining_time--;

                if (p->block_moment >= 0 && p->remaining_time > 0 && p->block_moment == (_time + 1)) {
                    p->state = BLOCK;
                    pcb_push(&_blocked, p, _time + 1);
                    log_state(_time + 1, p);
                    processors[i] = NULL;
                } else if (p->remaining_time <= 0) {
                    p->remaining_time = 0;
                    p->state = FINISH;
                    log_state(_time + 1, p);
                    pcb_push(&_finish, p, _time + 1);
                    processors[i] = NULL;
                }
            }
        }

        // Finaliza processos terminados
        while (1) {
            PCB *p = pcb_pop(&_finish);
            if (!p) break;
            free(p);
            nFinished++;
        }

        _time++;
    }

    free(buffer);
    return 0;
}
//gcc -std=c11 -Wall -O2 -o simso main.c pcb.c escalonador.c
//<mem_size> <quantum>|<num_processadores>|<processo_1>|<processo_2>|...
//processoN = <tempo_chegada> <tempo_execucao> [<momento_bloqueio>b<tempo_bloqueio>]
//./simso "2 2|2|3|0 5|1 3|2 4"
//./simso "2 0|2|3|0 5|1 3|2 4"