#include "sys_proc.h"
#include "k_rtx.h"
#include "k_process.h"

void timer_i_process ( ) {
	__disable_irq();
	while ( /*pending messages to i-process*/ 0 ) {
		//insert envelope into the timeout queue ;
	}
	while ( /*first message in queue timeout expired*/ 0 ) {
		//msg_t * env = dequeue ( timeout_queue ) ;
		//int target_pid = env->destination_pid ;
		// forward msg to destination
		__enable_irq();
		//send_message ( target_pid , env ) ;
	}
	__enable_irq();
	k_release_processor();
}

void uart_i_process ( ) {
	while (1) {}
}
