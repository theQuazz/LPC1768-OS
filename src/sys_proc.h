#ifndef _SYS_PROC_H_
#define _SYS_PROC_H_


void set_sys_procs(void);


void timer_i_process(void);
void uart_i_process(void);
void uart_i_process_ih(void);
void k_read_character(char);



#endif
