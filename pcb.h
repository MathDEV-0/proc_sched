#ifndef PCB_H
#define PCB_H

enum States {
    CREATE,
    READY,
    RUN,
    FINISH,
    BLOCK,
    SUS_BLOCK,
    SUS_READY
};

extern const char* states[];

typedef struct PCB {
    int id;
    unsigned char state;
    int remaining_time;
    int block_moment;
    int block_time;
    struct PCB *next;
} PCB;

PCB* pcb_new();
void pcb_push(PCB **queue, PCB *item, int t);
PCB* pcb_pop(PCB **queue);
void log_state(int t, PCB *p);

#endif
