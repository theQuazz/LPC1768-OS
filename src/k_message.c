#include "k_message.h"

/**
 * @brief enqueue
 */
void enqueue(MessageQueue *q, MSG *p) {
  if (!q->last) q->last = p;
  if (q->first) q->first->prev = p;
  p->next = q->first;
  p->prev = NULL;
  q->first = p;
}

/**
 * @brief remove
 */
MSG *queue_remove(MessageQueue *q, int pid) {
  MSG *p = q->first;

	while (p) {
		if (p->sender_pid == pid) {
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

MessageQueue process_message_queues[NUM_PROCESSES];

int k_send_message(int pid, void *envelope){
	MSG *m = envelope;
	m->sender_pid = k_get_current_pid();
	m->destination_pid = pid;
	m->mtype = DEFAULT;

	enqueue(process_message_queues[pid], m);
	k_conditional_unblock_pid(pid, BLK_MSG);

	return 0;
}

void *k_receive_message(int *pid){
	int curr = k_get_current_pid();
	MSG *msg;
	
	while (msg = queue_remove(process_message_queue[curr], *pid) == NULL) {
		k_block_current_process(BLK_MSG);
	}
	return msg->data;
}

void *k_recieve_message_noblock(int *pid){
	int curr = k_get_current_pid();
	MSG *msg;
	
	return queue_remove(process_message_queue[curr], *pid);
}