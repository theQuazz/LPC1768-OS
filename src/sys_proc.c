#include "sys_proc.h"
#include "k_rtx.h"
#include "k_process.h"
#include "k_message.h"
#include "printf.h"
#include <LPC17xx.h>

int message_timed_out_predicate(MSG *msg, void *v) {
	return 1; /*msg->timeout >= g_timer_count;*/
}

void timer_i_process ( ) {
	MSG *msg;

	while ( /*msg = message_queue_remove(&message_queues..., message_timed_out_predicate, NULL)*/ 0 ) {
		__enable_irq();
		//send_message ( target_pid , env ) ;
	}
}

void uart_i_process ( ) {
	uint8_t IIR_IntId;	    // Interrupt ID from IIR 		 
	LPC_UART_TypeDef *pUart = (LPC_UART_TypeDef *)LPC_UART0;
	char c;

	/* Reading IIR automatically acknowledges the interrupt */
	IIR_IntId = (pUart->IIR) >> 1 ; // skip pending bit in IIR 
	if (IIR_IntId & IIR_RDA) { // Receive Data Avaialbe
		/* read UART. Read RBR will clear the interrupt */
		c = pUart->RBR;
		
		// TODO: send message to proc_KCD
		
#ifdef DEBUG_0
		printf("Reading char %c\n\r", c);
#endif // DEBUG_0
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
