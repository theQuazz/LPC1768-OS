#include "sys_proc.h"
#include "k_rtx.h"
#include "k_process.h"
#include "k_message.h"
#include "timer.h"

extern MessageQueue process_message_queues[NUM_PROCS];

extern volatile uint32_t g_timer_count;

void timer_i_process ( ) {
	MSG *msg;
	
	__disable_irq();

 	msg = process_message_queues[TIMER_I_PROCESS_PID].first;

	while ( msg ) {
		if ( msg->timeout >= g_timer_count ) {
			if (msg->next) msg->next->prev = msg->prev;
			if (msg->prev) msg->prev->next = msg->next;
			
			msg->next = msg->prev = NULL;

			k_send_message(msg->destination_pid, msg->usr_msg);
		}
		msg = msg->next;
	}

	__enable_irq();
	k_release_processor();
}

void uart_i_process ( ) {
	while (1) {}
}
