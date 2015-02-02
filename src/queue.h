#include "stddef.h"

typedef struct queue {
  struct q_node *first;
  struct q_node *last;
} Queue;

typedef struct q_node {
  struct q_node *next;
  struct q_node *prev;
  void *data;
} QNode;

void enqueue(Queue *q, QNode *item);
QNode *dequeue(Queue *q);
