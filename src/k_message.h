int k_send_message(int, void*);
void *k_receive_message(int*);

typedef struct msg_t {
	void *next_msg;
	int sender_pid;
	int destination_pid;
	int message_type;
	char message_data[1];
} MSG;
