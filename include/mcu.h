#ifndef __CPU_H__
#define __CPU_H__

#include "memory.h"
typedef struct _8051mcu_t {
  memory_t *mem;
  addr_t   pc;
  int      cycles;
} mcu_t;

mcu_t mcu;

void mcu_init(mcu_t *mcu);

#endif // !__CPU_H__