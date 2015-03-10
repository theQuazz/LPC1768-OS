#include "sys_proc.h"
#include "k_rtx.h"
#include "k_process.h"
#include "k_message.h"

int message_timed_out_predicate(MSG *msg, void *v) {
	return 1; /*msg->timeout >= gp_current_time;*/
}

void timer_i_process ( ) {
	MSG *msg;

	__disable_irq();

	while ( /*msg = message_queue_remove(&message_queues..., message_timed_out_predicate, NULL)*/ 0 ) {
		__enable_irq();
		//send_message ( target_pid , env ) ;
	}
	__enable_irq();
	k_release_processor();
}

void uart_i_process ( ) {
	while (1) {}
}
