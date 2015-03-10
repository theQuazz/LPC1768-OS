#include "sys_proc.h"
#include "k_rtx.h"
#include "k_process.h"
#include "k_memory.h"
#include "k_message.h"
#include "printf.h"
#include "uart.h"
#include "timer.h"
#include <LPC17xx.h>

extern MessageQueue process_message_queues[NUM_PROCS];

extern volatile uint32_t g_timer_count;

int message_timed_out_predicate(MSG *msg, void *v) {
	return msg->timeout <= g_timer_count;
}

void timer_i_process ( ) {
	MSG *msg;

	while ( msg = msg_queue_remove(&process_message_queues[TIMER_I_PROCESS_PID], message_timed_out_predicate, NULL) ) {
		msg_enqueue(&process_message_queues[msg->destination_pid], msg);
		k_conditional_unblock_pid(msg->destination_pid, BLK_MSG);
	}
}

void read_character(char c) {
	KCD_MSG *m;
	switch (c) {
#ifdef DEBUG_HOTKEYS
		case '!':
			k_print_queue(RDY);
			break;
		case '@':
			k_print_queue(BLK_MEM);
			break;
		case '#':
			k_print_queue(BLK_MSG);
			break;
#endif
		default:
			m = k_request_memory_block();
			m->mtype = KCD_REG;
			m->body[0] = c;
			k_send_message(KCD_PROCESS_PID, m);
	}

#ifdef DEBUG_0
		printf("Reading char %c\n\r", c);
#endif // DEBUG_0
}

void uart_i_process ( ) {
	uint8_t IIR_IntId;	    // Interrupt ID from IIR 		 
	LPC_UART_TypeDef *pUart = (LPC_UART_TypeDef *)LPC_UART1;

	/* Reading IIR automatically acknowledges the interrupt */
	IIR_IntId = (pUart->IIR) >> 1 ; // skip pending bit in IIR 
	if (IIR_IntId & IIR_RDA) { // Receive Data Avaialbe
		/* read UART. Read RBR will clear the interrupt */
		read_character(pUart->RBR);
	} else if (IIR_IntId & IIR_THRE) {
		/* THRE Interrupt, transmit holding register becomes empty */
		
		// TODO: read next character in buffer

		pUart->IER ^= IER_THRE; // toggle the IER_THRE bit 
		pUart->THR = '\0';
	} else {  /* not implemented yet */
#ifdef DEBUG_0
			printf("Should not get here!\n\r");
#endif // DEBUG_0
		return;
	}
	
	// TODO: recieve messages and put them into the buffer
}
