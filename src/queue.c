#include "queue.h"

void enqueue(Queue *q, QNode *item) {
  if (!q->last) q->last = item;
  item->next = q->first;
  item->prev = NULL;
  q->first->prev = item;
  q->first = item;
}

QNode *dequeue(Queue *q) {
  QNode *tmp = q->last;
  q->last = q->last->prev;
  q->last->next = NULL;
  return tmp;
}
