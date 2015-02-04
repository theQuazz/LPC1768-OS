/**
 * @file:   k_process.c  
 * @brief:  process management C file
 * @author: Yiqing Huang
 * @author: Thomas Reidemeister
 * @date:   2014/01/17
 * NOTE: The example code shows one way of implementing context switching.
 *       The code only has minimal sanity check. There is no stack overflow check.
 *       The implementation assumes only two simple user processes and NO HARDWARE INTERRUPTS. 
 *       The purpose is to show how context switch could be done under stated assumptions. 
 *       These assumptions are not true in the required RTX Project!!!
 *       If you decide to use this piece of code, you need to understand the assumptions and
 *       the limitations. 
 */

#include <LPC17xx.h>
#include <system_LPC17xx.h>
#include "uart_polling.h"
#include "k_process.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* ----- Global Variables ----- */
PCB **gp_pcbs;                  /* array of pcbs */
PCB *gp_current_process = NULL; /* always point to the current RUN process */
PCB *gp_null_process = NULL;    /* always point to the null process */

PriorityQueue gp_priority_queues[NUM_PROC_STATE];

/* process initialization table */
PROC_INIT g_proc_table[NUM_PROCS];
extern PROC_INIT g_test_procs[NUM_TEST_PROCS];

void k_block_current_process(PROC_STATE_E state) {
	PCBQueue *q = &gp_priority_queues[state].priorities[gp_current_process->m_priority];
	gp_current_process->m_state = state;
	enqueue(q, gp_current_process);
	k_release_processor();
}

void k_unblock_from_queue(PROC_STATE_E blocked_queue) {
	PriorityQueue *q = &gp_priority_queues[blocked_queue];
	PCB *p = scheduler(q);
	if (p) {
		enqueue(&gp_priority_queues[RDY].priorities[p->m_priority], p);
		p->m_state = RDY;
	}
}

/**
 * @biref: initialize all processes in the system
 * NOTE: We assume there are only two user processes in the system in this example.
 */
void process_init() 
{
	int i;
	U32 *sp;
  
        /* fill out the initialization table */
	set_test_procs();

	g_proc_table[0].m_pid = 0;
	g_proc_table[0].m_stack_size = 8;
	g_proc_table[0].mpf_start_pc = null_process;
	g_proc_table[0].m_priority = NUM_PRIORITIES;

	for ( i = 1; i < NUM_PROCS; i++ ) {
		g_proc_table[i].m_pid = g_test_procs[i - 1].m_pid;
		g_proc_table[i].m_stack_size = g_test_procs[i - 1].m_stack_size;
		g_proc_table[i].mpf_start_pc = g_test_procs[i - 1].mpf_start_pc;
		g_proc_table[i].m_priority = g_test_procs[i - 1].m_priority;
	}
  
	/* initilize exception stack frame (i.e. initial context) for each process */
	for ( i = 0; i < NUM_PROCS; i++ ) {
		int j;
		(gp_pcbs[i])->m_pid = (g_proc_table[i]).m_pid;
		(gp_pcbs[i])->m_priority = (g_proc_table[i]).m_priority;
		(gp_pcbs[i])->m_state = NEW;
		
		sp = alloc_stack((g_proc_table[i]).m_stack_size);
		*(--sp)  = INITIAL_xPSR;      // user process initial xPSR  
		*(--sp)  = (U32)((g_proc_table[i]).mpf_start_pc); // PC contains the entry point of the process
		for ( j = 0; j < 6; j++ ) { // R0-R3, R12 are cleared with 0
			*(--sp) = 0x0;
		}
		(gp_pcbs[i])->mp_sp = sp;
	}
	
	/* queue procs */
	for ( i = 0; i < NUM_TEST_PROCS; i++ ) {
		enqueue(&gp_priority_queues[RDY].priorities[gp_pcbs[i + 1]->m_priority], gp_pcbs[i + 1]);
	}

	gp_current_process = gp_null_process = gp_pcbs[0];
}

/*@brief: scheduler, pick the pid of the next to run process
 *@return: PCB pointer of the next to run process
 *         NULL if error happens
 *POST: if gp_current_process was NULL, then it gets set to pcbs[0].
 *      No other effect on other global variables.
 */

PCB *scheduler(PriorityQueue *queue)
{
	int i;
	PCB *p;

	for (i = 0; i < NUM_PRIORITIES; i++) {
		p = dequeue(&(queue->priorities[i]));
		if (p) {
			return p;
		}
	}
	
	return NULL;
}

/*@brief: switch out old pcb (p_pcb_old), run the new pcb (gp_current_process)
 *@param: p_pcb_old, the old pcb that was in RUN
 *@return: RTX_OK upon success
 *         RTX_ERR upon failure
 *PRE:  p_pcb_old and gp_current_process are pointing to valid PCBs.
 *POST: if gp_current_process was NULL, then it gets set to pcbs[0].
 *      No other effect on other global variables.
 */
int process_switch(PCB *p_pcb_old) 
{
	PROC_STATE_E state;
	
	state = gp_current_process->m_state;

	if (state == NEW) {
		if (gp_current_process != p_pcb_old && p_pcb_old->m_state != NEW) {
			if (p_pcb_old->m_state == RUN) {
				p_pcb_old->m_state = RDY;
			}
			p_pcb_old->mp_sp = (U32 *) __get_MSP();
		}
		gp_current_process->m_state = RUN;
		__set_MSP((U32) gp_current_process->mp_sp);
		__rte();  // pop exception stack frame from the stack for a new processes
	} 
	
	/* The following will only execute if the if block above is FALSE */

	if (gp_current_process != p_pcb_old) {
		if (state == RDY) {
			if (p_pcb_old->m_state == RUN) {
				p_pcb_old->m_state = RDY;
			}
			p_pcb_old->mp_sp = (U32 *) __get_MSP(); // save the old process's sp
			gp_current_process->m_state = RUN;
			__set_MSP((U32) gp_current_process->mp_sp); //switch to the new proc's stack    
		} else {
			gp_current_process = p_pcb_old; // revert back to the old proc on error
			return RTX_ERR;
		} 
	}
	return RTX_OK;
}
/**
 * @brief release_processor(). 
 * @return RTX_ERR on error and zero on success
 * POST: gp_current_process gets updated to next to run process
 */
int k_release_processor(void)
{
	PCB *p_pcb_old = NULL;
	
	p_pcb_old = gp_current_process;

  if ( p_pcb_old != gp_null_process && p_pcb_old->m_state == RUN ) {
		enqueue(&gp_priority_queues[RDY].priorities[p_pcb_old->m_priority], p_pcb_old);
	}

	gp_current_process = scheduler(&gp_priority_queues[RDY]);
	
	if ( gp_current_process == NULL  ) {
		gp_current_process = gp_null_process;
	}

	process_switch(p_pcb_old);

	return RTX_OK;
}

/**
 * @brief null_process()
 */
void null_process() {
	while (1) {
		k_release_processor();
	}
}

/**
 * @brief enqueue
 */
void enqueue(PCBQueue *q, PCB *p) {
  if (!q->last) q->last = p;
  if (q->first) q->first->prev = p;
  p->next = q->first;
  p->prev = NULL;
  q->first = p;
}

/**
 * @brief dequeue
 */
PCB *dequeue(PCBQueue *q) {
  PCB *tmp = q->last;
  if (!tmp) return NULL;
  q->last = tmp->prev;
  if (q->last) q->last->next = NULL;
  return tmp;
}

/**
 * @brief remove
 */
PCB *queue_remove(PCBQueue *q, int pid) {
  PCB *p = q->first;

	if (!p) return NULL;

	do {
		
		if (p->m_pid == pid) {
			if (p->next) {
				p->next->prev = p->prev;
			} else {
				q->last = p->prev;
			}

			if (p->prev) {
				p->prev->next = p->next;
			} else {
				q->first = p->next;
			}

			return p;
		}

		p = p->next;
	} while(p);
	
	return NULL;
}

/**
 * @brief k_set_process_priority
 */
int k_set_process_priority(int pid, int priority) {
	PCB *p = gp_pcbs[pid];
	int state;

  if (pid == 0 && priority == NULL_PROIRITY) {
    return RTX_OK;
  }
	
	if (!( 0 < pid && pid < NUM_PROCS && HIGH_PRIORITY <= priority && priority <= LOWEST_PRIORITY )) {
#ifdef DEBUG_0
		printf("somethin ain't right, trying to set_process_priority(%d, %d)\r\n", pid, priority);
#endif
		return RTX_ERR;
	}

	state = p->m_state;
	
	// if state is the NEW state it is in the RDY queue
	if (state == NEW) {
		state = RDY;
	}

	if (gp_current_process->m_pid != pid) {
		queue_remove(&gp_priority_queues[state].priorities[p->m_priority], pid);
		enqueue(&gp_priority_queues[state].priorities[priority], p);
	}

	p->m_priority = priority;

  return RTX_OK;
}

/**
 * @brief k_get_process_priority
 */
int k_get_process_priority(int pid) {
	if (!( 0 <= pid && pid < NUM_PROCS )) {
#ifdef DEBUG_0
		printf("somethin ain't right, trying to get_process_priority(%d)\r\n", pid);
#endif
		return RTX_ERR;
	}

	return gp_pcbs[pid]->m_priority;
}
