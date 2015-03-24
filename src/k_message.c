#include "k_message.h"
#include "timer.h"

extern volatile uint32_t g_timer_count;

MessageQueue process_message_queues[NUM_PROCS];

void msg_enqueue(MessageQueue *q, MSG *p) {
  if (!q->last) q->last = p;
  if (q->first) q->first->prev = p;
	else q->first = NULL;
  p->next = q->first;
  p->prev = NULL;
  q->first = p;
}

MSG *msg_queue_remove(MessageQueue *q, int (*pred)(MSG*, void*), void *data) {
  MSG *p = q->first;

	while (p) {
		if (pred(p, data)) {
			if (p->next) {
				p->next->prev = p->prev;
			} else {
				q->last = p->prev;
			}

			if (p->prev) {
				p->prev->next = p->next;
			} else {
				q->first = p->next;
			}

			return p;
		}

		p = p->next;
	}
	
	return NULL;
}

int message_pid_predicate(MSG *msg, void *vpid) {
	int *pid = vpid;
	return msg->sender_pid == *pid;
}

int message_always_true_predicate(MSG *msg, void *v) {
	return 1;
}

int k_send_message(int pid, void *envelope) {
	MSG *m = envelope;

	__disable_irq();
	
	if (!(0 <= pid && pid < NUM_PROCS)) {
		return RTX_ERR;
	}

	m->sender_pid = k_get_current_pid();
	m->destination_pid = pid;
	m->mtype = DEFAULT;

	msg_enqueue(&process_message_queues[pid], m);
	k_conditional_unblock_pid(pid, BLK_MSG);

	__enable_irq();
	return 0;
}

void *k_receive_message(int *pid) {
	int curr = k_get_current_pid();
	MSG *msg;
	
	__disable_irq();
	
	while ((msg = msg_queue_remove(&process_message_queues[curr], message_pid_predicate, pid)) == NULL) {
		k_block_current_process(BLK_MSG);
	}
	
	__enable_irq();
	return msg->usr_msg;
}

void *k_receive_first_message() {
	int curr = k_get_current_pid();
	MSG *msg;

	__disable_irq();
	
	while ((msg = msg_queue_remove(&process_message_queues[curr], message_always_true_predicate, NULL)) == NULL) {
		k_block_current_process(BLK_MSG);
	}

	__enable_irq();
	return msg->usr_msg;
}

void *k_recieve_message_noblock(int pid) {
	int curr = k_get_current_pid();
	
	return msg_queue_remove(&process_message_queues[curr], message_pid_predicate, &pid);
}

void *k_receive_first_message_nonblock() {
	int curr = k_get_current_pid();
	void *msg;

	__disable_irq();
	
	msg = msg_queue_remove(&process_message_queues[curr], message_always_true_predicate, NULL);
	
	__enable_irq();
	
	return msg;
}

int k_delayed_send(int pid, void *message_envelope, int delay)
{
	MSG *m = message_envelope;

	__disable_irq();

	m->timeout = delay + g_timer_count;
	m->sender_pid = k_get_current_pid();
	m->destination_pid = pid;
	m->mtype = DEFAULT;

	msg_enqueue(&process_message_queues[TIMER_I_PROCESS_PID], m);

	__enable_irq();
	return 0;
}
