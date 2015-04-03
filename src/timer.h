/**
* @brief timer.h - Timer header file
* @author Y. Huang
* @date 2013/02/12
*/
#ifndef _TIMER_H_
#define _TIMER_H_

#include "k_process.h"
#include <stdint.h>

#define TIMER_I_PROCESS_PID 14

extern uint32_t timer_init ( uint8_t n_timer ); /* initialize timer n_timer */
extern void k_timer_start ( void ); /* start timing in 10us increments */
extern int k_timer_end ( void );

#endif /* ! _TIMER_H_ */
