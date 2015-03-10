#include "k_process.h"
#include "k_rtx.h"

typedef enum {
	DEFAULT,
	KCD_REG,
	NUM_MSG_TYPES
} MSG_TYPES;
	

typedef struct msg_t {
	U8 usr_msg[20];
	struct msg_t *next;
	struct msg_t *prev;
	int sender_pid;
	int destination_pid;
	int mtype;
	int timeout;
} MSG;

typedef struct message_queue {
	MSG *first;
	MSG *last;
} MessageQueue;

int k_send_message(int, void*);
int k_delayed_send(int, void *, int);
void *k_receive_message(int);
void *k_recieve_message_noblock(int);
void msg_enqueue(MessageQueue *q, MSG *m);
MSG *msg_queue_remove(MessageQueue *q, int pid);
