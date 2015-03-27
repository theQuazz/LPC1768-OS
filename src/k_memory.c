/**
 * @file:   k_memory.c
 * @brief:  kernel memory managment routines
 * @author: Yiqing Huang
 * @date:   2014/01/17
 */

#include "k_memory.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

/* ----- Global Variables ----- */
U32 *gp_stack; /* The last allocated stack low address. 8 bytes aligned */
               /* The first stack starts at the RAM high address */
	       /* stack grows down. Fully decremental stack */
MEM_BLK *first_free_blk = NULL;

/**
 * @brief: Initialize RAM as follows:

0x10008000+---------------------------+ High Address
          |    Proc 1 STACK           |
          |---------------------------|
          |    Proc 2 STACK           |
          |---------------------------|<--- gp_stack
          |                           |
          |        HEAP               |
          |                           |
          |---------------------------|
          |        PCB 2              |
          |---------------------------|
          |        PCB 1              |
          |---------------------------|
          |        PCB pointers       |
          |---------------------------|<--- gp_pcbs
          |        Padding            |
          |---------------------------|  
          |Image$$RW_IRAM1$$ZI$$Limit |
          |...........................|          
          |       RTX  Image          |
          |                           |
0x10000000+---------------------------+ Low Address

*/

void memory_init(void)
{
	U8 *top_address = (U8 *)&Image$$RW_IRAM1$$ZI$$Limit;
	int i;
  
	/* 4 bytes padding */
	top_address += 4;

	/* allocate memory for pcb pointers   */
	gp_pcbs = (PCB **)top_address;
	top_address += (NUM_PROCS)* sizeof(PCB *);
  
	for (i = 0; i < NUM_PROCS; i++) {
		gp_pcbs[i] = (PCB *)top_address;
		top_address += sizeof(PCB);
	}
	
	first_free_blk = (MEM_BLK *) top_address;
	
	/* prepare for alloc_stack() to allocate memory for stacks */
	
	gp_stack = (U32 *)RAM_END_ADDR;
	if ((U32)gp_stack & 0x04) { /* 8 bytes alignment */
		--gp_stack; 
	}
}

void heap_init(void) {
	int i;
	int num_blks;
	MEM_BLK *mem;

	/* allocate memory for heap, not implemented yet*/
// 	num_blks = ((U8 *)gp_stack - (U8 *)first_free_blk) / sizeof(MEM_BLK) - 1;
	num_blks = 30;
  mem = first_free_blk;

  for (i = 0; i < num_blks - 1; i++) {
    mem[i].next = &mem[i + 1];
    mem[i].free = 1;
  }

  mem[num_blks - 1].next = NULL;
  mem[num_blks - 1].free = 1;
}

/**
 * @brief: allocate stack for a process, align to 8 bytes boundary
 * @param: size, stack size in bytes
 * @return: The top of the stack (i.e. high address)
 * POST:  gp_stack is updated.
 */

U32 *alloc_stack(U32 size_b) 
{
	U32 *sp;
	sp = gp_stack; /* gp_stack is always 8 bytes aligned */
	
	/* update gp_stack */
	gp_stack = (U32 *)((U8 *)sp - size_b);
	
	/* 8 bytes alignement adjustment to exception stack frame */
	if ((U32)gp_stack & 0x04) {
		--gp_stack; 
	}
	return sp;
}

void *k_request_memory_block(void) {
	MEM_BLK *first;

	__disable_irq();
#ifdef DEBUG_0 
	//printf("k_request_memory_block: entering...\r\n");
#endif /* ! DEBUG_0 */

  while (!(first = first_free_blk)) {
		k_block_current_process(BLK_MEM);
	}
	
  first_free_blk->free = 0;
  first_free_blk       = first_free_blk->next;

	__enable_irq();
  return first;
}

int k_release_memory_block(void *memory_block) {
	MEM_BLK *first = memory_block;
	
	__disable_irq();

#ifdef DEBUG_0 
	//printf("k_release_memory_block: releasing block @ 0x%x\r\n", memory_block);
#endif /* ! DEBUG_0 */

  if (first->free) {
		return RTX_ERR;
	}

  first->next    = first_free_blk;
  first->free    = 1;
  first_free_blk = first;
	
	k_unblock_from_queue(BLK_MEM);
	
	__enable_irq();

  return RTX_OK;
}
