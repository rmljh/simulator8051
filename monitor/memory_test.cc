#include "../include/memory.h"

void memory_test() {
  word_t data[MEM_TYPE_XRAM];
  
  for (int i = 0; i < 128; ++i) { // test bit
    memory_write(i, 1, MEM_TYPE_IRAM);
    data[i] = memory_read(i, MEM_TYPE_IRAM);
  }

  for (int i = 128; i < 256; ++i) { // test sfr
    memory_write(i, 1, )
  }
}