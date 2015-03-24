#include "rtx.h"
#include "printf.h"
#include "uart.h"
#include <LPC17xx.h>

volatile GEN_MSG *buffer;
volatile int buffer_pos = 0;

void uart_i_process() {
	LPC_UART_TypeDef *pUart = (LPC_UART_TypeDef *)LPC_UART1;
	GEN_MSG *msg;
	
	while (msg = receive_first_message()) {
		while (buffer_pos != 0) {}
		buffer = msg;
		buffer_pos = 1;
		pUart->THR = buffer->body[0];
		pUart->IER ^= IER_THRE;
	}
}
