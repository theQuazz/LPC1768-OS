#include "mem_management.h"

#ifdef STATIC_HEAP
byte  heap[0xE000];
const byte *START_ADDR = heap;
const byte *END_ADDR   = heap + 0xE000;
#else
const byte *START_ADDR = (void *) Image$$RW_IRAM1$$ZI$$Limit;
const byte *END_ADDR   = (void *) 0x10007FFF;
#endif

struct mem_blk *first_free_blk = NULL;

void init_memory() {
  const int num_blks  = (END_ADDR - START_ADDR) / sizeof(struct mem_blk);
  struct mem_blk *mem = (struct mem_blk*) START_ADDR;

  first_free_blk = &mem[0];

  for (int i = 0; i < num_blks - 1; i++) {
    mem[i].next = &mem[i + 1];
    mem[i].free = true;
  }

  mem[num_blks - 1].next = NULL;
  mem[num_blks - 1].free = true;
}

// TODO this needs some more work
int release_memory_block(void *memory_block) {
  struct mem_blk *first = memory_block;

  if (first->free) return 1;

  first->next    = first_free_blk;
  first->free    = true;
  first_free_blk = first;

  return 0;
}

void *request_memory_block() {
  struct mem_blk *first = first_free_blk;

  if (!first) return NULL;

  first_free_blk = first->next;
  first->free    = false;

  return first;
}
