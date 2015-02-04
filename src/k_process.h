/**
 * @file:   k_process.h
 * @brief:  process management hearder file
 * @author: Yiqing Huang
 * @author: Thomas Reidemeister
 * @date:   2014/01/17
 * NOTE: Assuming there are only two user processes in the system
 */

#ifndef K_PROCESS_H_
#define K_PROCESS_H_

#include "k_rtx.h"

/* ----- Definitions ----- */

#define INITIAL_xPSR 0x01000000        /* user process initial xPSR value */

/* ----- Functions ----- */

void process_init(void);                					/* initialize all procs in the system */
PCB *scheduler(PriorityQueue *);             			/* pick the pid of the next to run process */
int has_higher_priority_process(PriorityQueue *); /* */
int k_release_processor(void);          					/* kernel release_process function */
void null_process(void);                					/* np */
void enqueue(PCBQueue *, PCB *);        					/* enqueue a pcb */
PCB *dequeue(PCBQueue *);               					/* dequeue a pcb */
PCB *queue_remove(PCBQueue *, int pid); 					/* dequeue a pcb */
void k_block_current_process(PROC_STATE_E);				/* */
void k_unblock_from_queue(PROC_STATE_E);					/* */
int k_set_process_priority(int, int);							/* */		
int k_get_process_priority(int);									/* */

extern U32 *alloc_stack(U32 size_b);    /* allocate stack for a process */
extern void __rte(void);                /* pop exception stack frame */
extern void set_test_procs(void);       /* test process initial set up */

#endif /* ! K_PROCESS_H_ */
