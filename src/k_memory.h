/**
 * @file:   k_memory.h
 * @brief:  kernel memory managment header file
 * @author: Yiqing Huang
 * @date:   2014/01/17
 */
 
#ifndef K_MEM_H_
#define K_MEM_H_

#include "k_rtx.h"
#include "k_process.h"

/* ----- Definitions ----- */
#define RAM_END_ADDR 0x10008000
#define BLK_SIZE 128

/* ----- Variables ----- */
/* This symbol is defined in the scatter file (see RVCT Linker User Guide) */  
extern unsigned int Image$$RW_IRAM1$$ZI$$Limit; 
extern PCB **gp_pcbs;
extern PROC_INIT g_proc_table[NUM_TEST_PROCS];

typedef struct mem_blk {
  U8 blk[BLK_SIZE];
  struct mem_blk *next;
  unsigned char free;
} MEM_BLK;

/* ----- Functions ------ */
void memory_init(void);
void heap_init(void);
U32 *alloc_stack(U32 size_b);
void *k_request_memory_block(void);
int k_release_memory_block(void *);

#endif /* ! K_MEM_H_ */
