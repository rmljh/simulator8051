#ifndef __CPU_H__
#define __CPU_H__

#include "memory.h"
typedef struct _simu8051_t {
  memory_t* mem;
  addr_t    pc;
  int       cycles;
} simu8051_t;

extern simu8051_t mcu;

#endif // !__CPU_H__