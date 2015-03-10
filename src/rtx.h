/* @brief: rtx.h User API prototype, this is only an example
 * @author: Yiqing Huang
 * @date: 2014/01/17
 */
#ifndef RTX_H_
#define RTX_H_

/* ----- Definitations ----- */
#define RTX_ERR -1
#define NULL 0
#define NUM_TEST_PROCS 13
/* Process Priority. The bigger the number is, the lower the priority is*/
#define HIGH      0
#define MEDIUM    1
#define LOW       2
#define LOWEST    3
#define NUM_PRIOS 4
/* Message types */
#define DEFAULT 0
#define KCD_REG 1

/* ----- Types ----- */
typedef unsigned int U32;

/* initialization table item */
typedef struct proc_init
{	
	int m_pid;	        /* process id */ 
	int m_priority;         /* initial priority, not used in this example. */ 
	int m_stack_size;       /* size of stack in words */
	void (*mpf_start_pc) ();/* entry point of the process */    
} PROC_INIT;

typedef struct kcd_msg_t {
	int mtype;
	char body[1];
} KCD_MSG;

/* ----- RTX User API ----- */
#define __SVC_0  __svc_indirect(0)

extern void k_rtx_init(void);
#define rtx_init() _rtx_init((U32)k_rtx_init)
extern void __SVC_0 _rtx_init(U32 p_func);

extern int k_release_processor(void);
#define release_processor() _release_processor((U32)k_release_processor)
extern int __SVC_0 _release_processor(U32 p_func);

extern void *k_request_memory_block(void);
#define request_memory_block() _request_memory_block((U32)k_request_memory_block)
extern void *_request_memory_block(U32 p_func) __SVC_0;
/* __SVC_0 can also be put at the end of the function declaration */

extern int k_release_memory_block(void *);
#define release_memory_block(p_mem_blk) _release_memory_block((U32)k_release_memory_block, p_mem_blk)
extern int _release_memory_block(U32 p_func, void *p_mem_blk) __SVC_0;

extern int k_get_process_priority(int);
#define get_process_priority(pid) _get_process_priority((U32)k_get_process_priority, pid)
extern int _get_process_priority(U32 p_func, int pid) __SVC_0;

extern int k_set_process_priority(int, int);
#define set_process_priority(pid, priority) _set_process_priority((U32)k_set_process_priority, pid, priority)
extern int _set_process_priority(U32 p_func, int pid, int priority) __SVC_0;

extern int k_send_message(int, void*);
#define send_message(pid, msg) _send_message((U32)k_send_message, pid, msg)
extern int _send_message(U32 p_func, int pid, void *msg) __SVC_0;

extern void *k_receive_message(int);
#define receive_message(pid) _receive_message((U32)k_receive_message, pid)
extern void *_receive_message(U32 p_func, int pid) __SVC_0;

extern void *k_recieve_first_message_nonblock();
#define recieve_first_message_nonblock() _recieve_first_message_nonblock((U32)k_recieve_first_message_nonblock)
extern void *_recieve_first_message_nonblock(U32 p_func) __SVC_0;

extern void *k_recieve_first_message();
#define recieve_first_message() _recieve_first_message((U32)k_recieve_first_message)
extern void *_recieve_first_message(U32 p_func) __SVC_0;

#endif /* !RTX_H_ */
