#include "k_process.h"
#include "k_rtx.h"

typedef struct msg_t {
	U8 usr_msg[20];
	struct msg_t *next;
	struct msg_t *prev;
	int sender_pid;
	int destination_pid;
	int mtype;
} MSG;

typedef struct message_queue {
	MSG *first;
	MSG *last;
} MessageQueue;

int k_send_message(int, void*);
void *k_receive_message(int*);