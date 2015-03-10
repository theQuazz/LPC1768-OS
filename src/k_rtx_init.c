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
#include "uart.h"
#include "uart_polling.h"

void k_rtx_init(void)
{
        __disable_irq();

				uart0_init();
				uart1_irq_init();
				
#ifdef DEBUG_0
				init_printf(NULL, putc);
#endif /* DEBUG_0 */

        memory_init();
        process_init();
				timer_init(0);	
				heap_init();

        __enable_irq();
	
	/* start the first process */
	
        k_release_processor();
}
