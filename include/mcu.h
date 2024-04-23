#ifndef __CPU_H__
#define __CPU_H__

#include "memory.h"

// simulates the mcu of STC-B
typedef struct _8051mcu_t {
  memory_t *mem;
  dword_t   pc;
  int       cycles;
  bool      in_interrupt;
} mcu_t;

extern mcu_t mcu;

void mcu_init (mcu_t* mcu);
void mcu_reset(mcu_t* mcu);

// simulates relevant peripherals: timer, interrupt ...
void timer_update(int cycles);
void interrupt_controller();
void interrupt_entry(addr_t entry);
#endif // !__CPU_H__