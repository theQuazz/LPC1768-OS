#include "k_process.h"
#include "k_rtx.h"

typedef enum {
	DEFAULT,
	KCD_REG,
	NUM_MSG_TYPES
} MSG_TYPES;
	

typedef struct msg_t {
	U8 usr_msg[32];
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
void *k_receive_message(int*);
void *k_receive_first_message(void);
void *k_receive_message_noblock(int);
void *k_receive_first_message_nonblock(void);
void msg_enqueue(MessageQueue *q, MSG *m);
MSG *msg_queue_remove(MessageQueue *q, int (*pred)(MSG*, void*), void *data);
