/**
 * @file:   usr_proc.c
 * @brief:  Two user processes: proc1 and proc2
 * @author: Yiqing Huang
 * @date:   2014/01/17
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */

#include "rtx.h"
#include "uart_polling.h"
#include "usr_proc.h"
#include "string.h"
#include "stdlib.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

int num_tests = 0;
int num_tests_passed = 0;
int num_tests_failed = 0;

/* initialization table item */
PROC_INIT g_test_procs[NUM_TEST_PROCS];

void set_test_procs() {
	int i;
	for( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_test_procs[i].m_pid=(U32)(i+1);
		g_test_procs[i].m_priority=LOWEST;
		g_test_procs[i].m_stack_size=0x0200;
	}

	g_test_procs[0].mpf_start_pc = &proc1;
	g_test_procs[1].mpf_start_pc = &proc2;
	g_test_procs[2].mpf_start_pc = &proc3;
	g_test_procs[3].mpf_start_pc = &proc4;
	g_test_procs[4].mpf_start_pc = &proc5;
	g_test_procs[5].mpf_start_pc = &proc6;
	g_test_procs[6].mpf_start_pc = &proc_A;
	g_test_procs[7].mpf_start_pc = &proc_B;
	g_test_procs[8].mpf_start_pc = &proc_C;
	g_test_procs[9].mpf_start_pc = &set_process_priority_process;
	g_test_procs[10].mpf_start_pc = &wall_clock_display;
	g_test_procs[11].mpf_start_pc = &proc_KCD;
	g_test_procs[12].mpf_start_pc = &proc_CRT;
}


/**
 * @brief: a process that prints five uppercase letters
 *         and then yields the cpu.
 */
void proc1(void)
{
#ifdef DEBUG_0
	printf("G019_test: START\r\n");
#endif

	release_memory_block(receive_message(TEST_5_PID));
	release_memory_block(receive_message(TEST_3_PID));

#ifdef DEBUG_0
	printf("G019_test: %d/%d tests OK\r\n", num_tests_passed, num_tests);
	printf("G019_test: %d/%d tests FAIL\r\n", num_tests_failed, num_tests);
	printf("G019_test: END\r\n");
#endif

	receive_message(0);
}

struct msg_t {
	char body[1];
};

void proc2(void)
{
	struct msg_t *m = request_memory_block();
	m->body[0] = 'a';

	release_memory_block(receive_message(TEST_5_PID));

	delayed_send(3, m, 20);

	receive_message(0);
}

void proc3(void) {
	struct msg_t *m = receive_message(2);

#ifdef DEBUG_0
  num_tests++;
  if (m->body[0] == 'a') {
    printf("G019_test: test %d OK\r\n", 2);
    num_tests_passed++;
  } else {
    printf("G019_test: test %d FAIL\r\n", 2);
    num_tests_failed++;
  }
	release_memory_block(m);
#endif
	send_message(1, request_memory_block());

	receive_message(0);
}

/*
 * test 4, requires 2 processes
 */

int maxed_out_mem = 0;
int has_preempted = 1;

struct llnode {
  struct llnode *next;
  int data;
};

int free_ll(struct llnode *l) {
	struct llnode *tmp;
	while (l) {
		tmp = l->next;
		k_release_memory_block(l);
		l = tmp;
	}
	return 1;
}

void proc4(void) {
  int cache_size = 8;
  void *cache[8];
  int i;

  for ( i = 0; i < cache_size; i++ ) {
    cache[i] = request_memory_block();
  }

  set_process_priority(5, HIGH);

	delayed_send(TEST_4_PID, cache[0], 50);
	receive_message(TEST_4_PID);

  maxed_out_mem = 1;

  for ( i = 0; i < cache_size; i++ ) {
    release_memory_block(cache[i]);
		has_preempted = 0;
  }

  set_process_priority(4, LOWEST);
	receive_message(0);;
}

void proc5(void) {
  struct llnode *first = request_memory_block();
  struct llnode *tmp;

	first->next = NULL;

  do {
    tmp = request_memory_block();
    tmp->next = first;
    first = tmp;
  } while (!maxed_out_mem);

#ifdef DEBUG_0
  num_tests++;
	free_ll(first);
  if (has_preempted != 1) {
    printf("G019_test: test %d FAIL\r\n", 1);
    num_tests_failed++;
  } else {
    printf("G019_test: test %d OK\r\n", 1);
    num_tests_passed++;
  }
#endif
	send_message(TEST_1_PID, request_memory_block());
	send_message(TEST_2_PID, request_memory_block());

	receive_message(0);;
}

void proc6(void) {
	while (1) {}
}


void proc_A(void) {
	GEN_MSG *p;
	KCD_MSG *q;
	KCD_MSG *register_z = request_memory_block();
	int num = 0;

	register_z->mtype = KCD_REG;
	register_z->body[0] = 'Z';
	register_z->from = A_PID;
	send_message(KCD_PID, register_z);

	while (1) {
		p = receive_first_message();
		if (p->body[0] == 'Z') {
			release_memory_block(p);
			break;
		} else {
			release_memory_block(p);
		}
	}

	while (1) {
		q = request_memory_block();
		q->mtype = COUNT_REPORT;
		q->body[0] = num;
		send_message(B_PID, q);
		num++;
		release_processor();
	}
}

void proc_B(void) {
	while (1) {
		send_message(C_PID, receive_message(A_PID));
	}
}

typedef struct Queue {
	KCD_MSG *first;
	KCD_MSG *last;
} QUEUE;

void Queue_enque(struct Queue *q, KCD_MSG *item) {
  if (!q->last) q->last = item;
  if (q->first) q->first->prev = item;
  item->next = q->first;
  item->prev = NULL;
  q->first = item;
};

KCD_MSG *Queue_dequeue(struct Queue *q) {
  KCD_MSG *tmp = q->last;
  if (!tmp) return NULL;
  q->last = tmp->prev;
  if (q->last) q->last->next = NULL;
	if (q->first == tmp) q->first = NULL;
  return tmp;
}

void proc_C(void) {
	KCD_MSG *p;
	GEN_MSG *g;
	QUEUE q = { NULL, NULL };

	while (1) {
		if (!q.first) {
			p = receive_message(B_PID);
		} else {
			p = Queue_dequeue(&q);
		}
		if (p->mtype == COUNT_REPORT) {
			if (p->body[0] % 20 == 0) {
				g = (void *)p;
				strcpy(g->body, "Process C\r\n");
				g->length = strlen("Process C\r\n");
				send_message(CRT_PID, g);
				p = request_memory_block();
				p->mtype = WAKEUP_10;
				delayed_send(C_PID, p, 10000);
				while (1) {
					p = receive_first_message();
					if (p->mtype == WAKEUP_10) {
						break;
					} else {
						Queue_enque(&q, p);
					}
				}
			}
		}
		release_memory_block(p);
		release_processor();
	}
}

void set_process_priority_process(void) {
	KCD_MSG *register_c = request_memory_block();
	GEN_MSG *msg;
	char *pos;
	int pid, prio;
	int err;

	register_c->mtype = KCD_REG;
	register_c->body[0] = 'C';
	register_c->from = SET_PROCESS_PRIORITY_PID;
	send_message(KCD_PID, register_c);

	while (msg = receive_message(KCD_PID)) {
		pos = memchr(msg->body, ' ', msg->length);
		pos = strtok(pos, " ");
		if (pos == NULL) goto set_process_priority_error;
		pid = atoi(pos);
		pos = strtok(NULL, " ");
		if (pos == NULL) goto set_process_priority_error;
		prio = atoi(pos);
		release_memory_block(msg);
		err = set_process_priority(pid, prio);
		if (err == RTX_ERR) goto set_process_priority_error;
		continue;
	set_process_priority_error:
		release_memory_block(msg);
		msg = request_memory_block();
		strcpy(msg->body, "Invalid pid/priority\n\r");
		msg->length = strlen("Invalid pid/priority\n\r");
		send_message(CRT_PID, msg);
	}
}

void wall_clock_display(void) {
	GEN_MSG *msg;
	GEN_MSG *output;
	KCD_MSG *register_w = request_memory_block();
	int running = 0;
	long time = 0;
	int sec, min, hour;

	register_w->mtype = KCD_REG;
	register_w->body[0] = 'W';
	register_w->from = WALL_CLOCK_DISPLAY_PID;
	send_message(KCD_PID, register_w);

	while (msg = receive_first_message()) {
		if (msg->length == -1) {
			if (!running) continue;
			release_memory_block(msg);
			time += 1;
			output = request_memory_block();
			hour = (time / 3600) % 24;
			min  = (time / 60) % 60;
			sec  = time % 60;
			sprintf(output->body, "\r%d%d:%d%d:%d%d", hour / 10, hour % 10, min / 10, min % 10, sec / 10, sec % 10);
			output->length = strlen(output->body);
			send_message(CRT_PID, output);
			msg = request_memory_block();
			msg->length = -1;
			delayed_send(WALL_CLOCK_DISPLAY_PID, msg, 1000);
		} else if (strncmp(msg->body, "WS ", 3) == 0 && msg->length == 11) {
			release_memory_block(msg);
			hour = atoi(msg->body + 3);
			min  = atoi(msg->body + 6);
			sec  = atoi(msg->body + 9);
			time = sec + min * 60 + hour * 3600 - 1;
			if (!running) {
				running = 1;
				msg = request_memory_block();
				msg->length = -1;
				send_message(WALL_CLOCK_DISPLAY_PID, msg);
			}
		} else if (strncmp(msg->body, "WR", 2) == 0) {
			release_memory_block(msg);
			time = -1;
			if (!running) {
				running = 1;
				msg = request_memory_block();
				msg->length = -1;
				send_message(WALL_CLOCK_DISPLAY_PID, msg);
			}
		} else if (strncmp(msg->body, "WT", 2) == 0) {
			release_memory_block(msg);
			running = 0;
		} else {
			release_memory_block(msg);
		}
	}
}

enum kcd_state_t {
	NOTHING,
	PERCENT,
	READING
};

int registered_command_pids[128] = { 0 };

void proc_KCD(void) {
	KCD_MSG *msg;
	GEN_MSG *msg_gen;
	char *body;
	enum kcd_state_t state = NOTHING;
	GEN_MSG *command;
	int pid;

	msg_gen = request_memory_block();
	strcpy(msg_gen->body, "> ");
	msg_gen->length = 2;
	send_message(CRT_PID, msg_gen);
	msg_gen = NULL;

	while (msg = receive_first_message()) {
		if (msg->mtype == KCD_REG) {
			registered_command_pids[msg->body[0]] = msg->from;
			release_memory_block(msg);
		} else {
			msg_gen = ((GEN_MSG*) msg);
			body = msg_gen->body;
			switch (body[0]) {
				case '%':
					if (state == NOTHING) state = PERCENT;
					break;
				case 127:
					if (state == READING) {
						if (command->length > 1) {
							command->body[--command->length] = '\0';
						} else {
							release_memory_block(command);
							state = PERCENT;
						}
					} else if (state == PERCENT) {
						state = NOTHING;
					} else if (state == NOTHING) {
						msg_gen->body[0] = '\0';
					}
					break;
				case '\r':
					msg_gen->body[msg_gen->length++] = '\n';
					msg_gen->body[msg_gen->length++] = '>';
					msg_gen->body[msg_gen->length++] = ' ';
					if (state == READING) {
						command->body[command->length] = '\0';
						pid = registered_command_pids[command->body[0]];
						if (pid) {
							send_message(pid, command);
						} else {
							memmove(command->body + 11, command->body, command->length);
							memcpy(command->body, "\n\rinvalid: ", 11);
							command->length += 11;
							send_message(CRT_PID, command);
						}
					}
					state = NOTHING;
					break;
				default:
					if (state == PERCENT) {
						command = request_memory_block();
						command->body[0] = body[0];
						command->length = 1;
						state = READING;
					} else if (state == READING) {
						command->body[command->length++] = body[0];
					}
			}
			send_message(CRT_PID, msg_gen);
		}
	}
}

void proc_CRT(void) {
	GEN_MSG *msg;
	while (msg = receive_first_message()) {
		send_message(UART_I_PROCESS_PID, msg);
	}
}
