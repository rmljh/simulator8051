#include "../include/memory.h"

void memory_test() {
  word_t data[MEM_XRAM_SIZE];
  
  // test bit
  for (int i = 0; i < 128; ++i) { 
    memory_write(i, 1, MEM_TYPE_IRAM);
    data[i] = memory_read(i, MEM_TYPE_IRAM);
  }

  // test sfr
  for (int i = 128; i < 256; ++i) { 
    memory_write(i, 1, MEM_TYPE_SFR);
    data[i] = memory_read(i, MEM_TYPE_SFR);
  }

  // test xram
  for (int i = 0; i < MEM_XRAM_SIZE; ++i) {
    memory_write(i, i, MEM_TYPE_XRAM);
    data[i] = memory_read(i, MEM_TYPE_XRAM);
  }
} 