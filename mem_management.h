#include "stdbool.h"
#include "stddef.h"

#define BLK_SIZE 128

typedef unsigned char byte;

extern byte Image$$RW_IRAM1$$ZI$$Limit;

struct mem_blk {
  byte blk[BLK_SIZE];
  struct mem_blk *next;
  bool free;
};

void init_memory();

void *request_memory_block();

int release_memory_block(void *memory_block);
