#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcb.h"
#include "escalonador.h"

// Variáveis globais
PCB *_create, *_ready, *_running, *_finish, *_blocked;
int _memSize, _quantum, _nprocs, _time, _pcount;
int processors_count;  // Contador de processadores

// Função para executar os processos, agora considerando múltiplos processadores
void execute_process(int processor_id, PCB *process) {
    printf("%02d:P%d -> executing (%d) %d\n", processor_id, process->id + 1, process->remaining_time, processor_id);
}

int main(int argc, char *argv[]) {
    const char *input = (argc > 1) ? argv[1] : "2 0|3|0 5|0 3|0 4";  // Entrada padrão se nenhum argumento for passado

    char *buffer = malloc(strlen(input) + 1);
    strcpy(buffer, input);

    // melhora a leitura
    for (int i = 0; i < (int)strlen(buffer); i++) if (buffer[i] == '|') buffer[i] = '\n';

    char line[256];
    char *buffp = buffer;
    int count;

    // lê a memória e o quantum
    sscanf(buffp, "%[^\n]%n", line, &count);
    buffp += count + 1;
    sscanf(line, "%d %d", &_memSize, &_quantum);

    // lê o número de processadores
    sscanf(buffp, "%[^\n]%n", line, &count);
    buffp += count + 1;
    sscanf(line, "%d", &processors_count);  // Agora lê o número de processadores

    // inicialização
    _create = _ready = _running = _finish = _blocked = NULL;
    _time = 0;
    _pcount = 0;
    int nFinished = 0;

    // Lê o primeiro processo
    sscanf(buffp, "%[^\n]%n", line, &count);
    buffp += count + 1;
    int nextT = -1;
    sscanf(line, "%d", &nextT);

    // Array para armazenar os processadores
    PCB *processors[processors_count];
    memset(processors, 0, sizeof(processors));  // Inicializa todos como NULL

    while (nFinished < _nprocs) {
        // Criação de novos processos
        while (nextT > -1 && _time == nextT) {
            PCB *p = pcb_new();
            int params = sscanf(line, "%d %d %db%d", &nextT, &p->remaining_time, &p->block_moment, &p->block_time);
            if (params < 4) p->block_moment = -1;
            p->id = _pcount++;
            log_state(_time, p);
            pcb_push(&_create, p);

            if (sscanf(buffp, "%[^\n]%n", line, &count) >= 1) {
                buffp += count + 1;
                sscanf(line, "%d", &nextT);
            } else {
                nextT = -1;
            }
        }

        // Move processos de "create" para "ready"
        while (1) {
            PCB *p = pcb_pop(&_create);
            if (!p) break;
            p->state = READY;
            pcb_push(&_ready, p);
            log_state(_time, p);
        }

        // Se não houver processo rodando, despacha um novo
        if (_running == NULL && _ready != NULL) {
            PCB *p = schedule(ALG_FIFO, &_ready, _quantum);
            if (p) {
                p->state = RUN;
                // Atribui o processo ao processador cíclicamente
                int processor_id = _time % processors_count;
                processors[processor_id] = p;
                log_state(_time, p);
                execute_process(processor_id + 1, p);  // Executa o processo no processador
            }
        }

        // Executa o processo em execução
        if (_running != NULL) {
            PCB *p = pcb_pop(&_running);
            p->remaining_time--;

            // Se o processo precisa ser bloqueado
            if (p->block_moment >= 0 && p->remaining_time > 0 && p->block_moment == (_time + 1)) {
                p->state = BLOCK;
                pcb_push(&_blocked, p);
                log_state(_time + 1, p);
            }
            // Se o processo terminou
            else if (p->remaining_time <= 0) {
                p->remaining_time = 0;  // Garante que o tempo não seja negativo
                p->state = FINISH;
                log_state(_time + 1, p);
                pcb_push(&_finish, p);
            }
            // Caso o processo não tenha terminado, coloque-o de volta na fila de "ready"
            else {
                p->state = READY;
                pcb_push(&_ready, p);
                log_state(_time + 1, p);
            }
        }

        // Finaliza os processos terminados
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


//gcc -std=c11 -O2 -o simso main.c pcb.c escalonador.c