#include "../include/mcu.h"

void mcu_init(mcu_t *mcu) {
  memory_init(&memory);
  mcu->mem = &memory;
  mcu->pc  = 0;
  mcu->cycles = 0;
} 