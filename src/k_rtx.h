
/** 
 * @file:   k_rtx.h
 * @brief:  kernel deinitiation and data structure header file
 * @auther: Yiqing Huang
 * @date:   2014/01/17
 */
 
#ifndef K_RTX_H_
#define K_RTX_H_

/*----- Definitations -----*/

#define RTX_ERR -1
#define RTX_OK  0

#define NULL 0
#define NUM_TEST_PROCS 13
#define NUM_SYS_PROCS  2
#define NUM_PROCS      NUM_TEST_PROCS + NUM_SYS_PROCS + 1
#define NUM_PRIORITIES 4	
#define KCD_PROCESS_PID 12
#define CRT_PROCESS_PID 13
#define TIMER_I_PROCESS_PID 14
#define UART_I_PROCESS_PID 15

#ifdef DEBUG_0
#define USR_SZ_STACK 0x200         /* user proc stack size 512B   */
#else
#define USR_SZ_STACK 0x100         /* user proc stack size 218B  */
#endif /* DEBUG_0 */


typedef struct kcd_msg_t {
	int mtype;
	char body[1];
} KCD_MSG;

typedef struct gen_msg_t {
	char body[24];
	int length;
} GEN_MSG;

/*----- Types -----*/
typedef unsigned char U8;
typedef unsigned int U32;

/* proc prios */
typedef enum {
  HIGH_PRIORITY = 0,
	MEDIUM_PRIORITY,
	LOW_PRIORITY,
	LOWEST_PRIORITY,
	NULL_PROIRITY
} PROC_PRIORITY_E;

/* process states, note we only assume three states in this example */
typedef enum {
  NEW = 0,
  RDY,
  RUN,
  BLK_MEM,
	BLK_MSG,
  NUM_PROC_STATE
} PROC_STATE_E;

struct pcb_queue;

/*
  PCB data structure definition.
  You may want to add your own member variables
  in order to finish P1 and the entire project 
*/
typedef struct pcb 
{ 
	struct pcb *next;  			/* next pcb, not used in this example */  
	struct pcb *prev;  			/* next pcb, not used in this example */
	int m_priority;     		/* initial priority, not used in this example. */	
	U32 *mp_sp;							/* stack pointer of the process */
	U32 m_pid;							/* process id */
	PROC_STATE_E m_state;   /* state of the process */ 
} PCB;

/*
   PCBQueue data structure definition
*/
typedef struct pcb_queue {
  struct pcb *first;
  struct pcb *last;
} PCBQueue;

/*
   PriorityQueue data structure
*/
typedef struct priority_queue {
  PCBQueue priorities[NUM_PRIORITIES];
} PriorityQueue;

/* initialization table item */
typedef struct proc_init
{	
	int m_pid;	        /* process id */ 
	int m_priority;         /* initial priority, not used in this example. */ 
	int m_stack_size;       /* size of stack in words */
	void (*mpf_start_pc) ();/* entry point of the process */    
} PROC_INIT;

#endif // ! K_RTX_H_
