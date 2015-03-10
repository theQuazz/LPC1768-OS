/** 
 * @file:   k_rtx_init.c
 * @brief:  Kernel initialization C file
 * @auther: Yiqing Huang
 * @date:   2014/01/17
 */

#include "k_rtx_init.h"
#include "k_memory.h"
#include "k_process.h"
#include "timer.h"
#include "printf.h"

void k_rtx_init(void)
{
        __disable_irq();
        memory_init();
        process_init();
				heap_init();
				timer_init(0);	
        __enable_irq();
	
	/* start the first process */
	
        k_release_processor();
}
