#include "../include/memory.h"
#define Rs()   do { rs = BITS(Mr(MEM_SFR_PSW, MEM_TYPE_IRAM), 4, 3) << 3; } while(0)
void memory_test() {
  word_t data[MEM_XRAM_SIZE];
  // test bit
  for (int i = 0; i < 128; i++) {
    simu8051_write(MEM_TYPE_BIT, i, 1);
    data[i] = memory_read(MEM_TYPE_BIT, i);
    printf("%d ", data[i]);
  }
  printf("\n");
  for (int i = 128; i < 256; i++) {
    simu8051_write(MEM_TYPE_BIT, i, 1);
    data[i] = memory_read(MEM_TYPE_BIT, i);
    printf("%d ", data[i]);
  }
  printf("\n");

  // test iram
  for (int i = 0; i < 128; ++i) { 
    memory_write(i, i, MEM_TYPE_IRAM);
    data[i] = memory_read(i, MEM_TYPE_IRAM);
    printf("%d ", data[i]);
  }
  printf("\n");

  // test iram
  Mw(MEM_SFR_PSW, 0x18, MEM_TYPE_IRAM);
  int rs; Rs(); printf("rs = %d\n", rs);
  for (int i = 0; i < 128 - rs; ++i) { 
    memory_write(rs + i, i, MEM_TYPE_IRAM);
    data[i] = memory_read(i, MEM_TYPE_IRAM);
    printf("%d ", data[i]);
  }
  printf("\n");

  // test sfr
  for (int i = 128; i < 256; ++i) { 
    memory_write(i, 1, MEM_TYPE_SFR);
    data[i] = memory_read(i, MEM_TYPE_SFR);
    printf("%d ", data[i]);
  }
  printf("\n");

  // test xram
  for (int i = 0; i < MEM_XRAM_SIZE; ++i) {
    memory_write(i, i, MEM_TYPE_XRAM);
    data[i] = memory_read(i, MEM_TYPE_XRAM);
  }
} 

void regs_dump(void) {
  int rs; Rs();
  printf("\tr0=%2x, r1=%2x, r2=%2x, r3=%2x\n"
    "\tr4=%2x, r5=%2x, r6=%2x, r7=%2x,\n"
    "\ta=%2x,b=%2x,sp=%2x,dptr=%4x, \n"
    "\tpsw=%2x\n",
    memory_read(MEM_TYPE_IRAM, rs + 0),
    memory_read(MEM_TYPE_IRAM, rs + 1),
    memory_read(MEM_TYPE_IRAM, rs + 2),
    memory_read(MEM_TYPE_IRAM, rs + 3),
    memory_read(MEM_TYPE_IRAM, rs + 4),
    memory_read(MEM_TYPE_IRAM, rs + 5),
    memory_read(MEM_TYPE_IRAM, rs + 6),
    memory_read(MEM_TYPE_IRAM, rs + 7),
    memory_read(MEM_TYPE_SFR, MEM_SFR_ACC),
    memory_read(MEM_TYPE_SFR, MEM_SFR_B),
    memory_read(MEM_TYPE_SFR, MEM_SFR_SP),
    memory_read(MEM_TYPE_SFR, MEM_SFR_DPH) << 8 | memory_read(MEM_TYPE_SFR, MEM_SFR_DPL),
    memory_read(MEM_TYPE_SFR, MEM_SFR_PSW));
}

int main() {
  memory_test();
  regs_dump();
}