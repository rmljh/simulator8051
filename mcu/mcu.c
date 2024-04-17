#include "../include/mcu.h"

void mcu_reset(mcu_t* mcu) {
  mcu_init(mcu);
}

void mcu_init(mcu_t *mcu) {
  memory_init(&memory);
  mcu->mem = &memory;
  mcu->pc  = 0;
  mcu->cycles = 0;
  mcu->in_interrupt = 0;
} 

void timer_update(int cycles) {
  if (!Mr(MEM_SFR_TCON_TR0, MEM_TYPE_BIT)) return;
  word_t tmod = Mr(MEM_SFR_TMOD, MEM_TYPE_SFR); int count;
  switch (BITS(tmod, 1, 0)) {
    case 0x00:
    case 0x02:
    case 0x03: break;
    case 0x01:
      count = (Mr(MEM_SFR_TH0, MEM_TYPE_SFR) << 8) | Mr(MEM_SFR_TL0, MEM_TYPE_SFR);
      count += cycles;
      if (count >= 0x10000) {
        Mw(MEM_SFR_TCON_TF0, 1, MEM_TYPE_BIT);
        count = 0;
      } 
      Mw(MEM_SFR_TH0, BITS(count, 15, 8), MEM_TYPE_SFR);
      Mw(MEM_SFR_TL0, BITS(count, 7,  0), MEM_TYPE_SFR);
      break;
  } 
}

void interrupt_controller() {
  word_t ie = Mr(MEM_SFR_IE, MEM_TYPE_SFR);
  if (!BITS(ie, 7, 7))    return;
  if (!mcu.in_interrupt)  return;

  // timer0 interrupt
  if(BITS(ie, 1, 1) && Mr(MEM_SFR_TCON_TF0, MEM_TYPE_BIT)) {
    interrupt_enter(INT_TIMER0_ENTRY);
    Mw(MEM_SFR_TCON_TF0, 0, MEM_TYPE_BIT);
  }
}

void interrupt_enter(addr_t entry) {
  word_t sp = Mr(MEM_SFR_SP, MEM_TYPE_SFR);
  mcu.in_interrupt = 1;
  Mw(++sp, BITS(mcu.pc, 7,  0), MEM_TYPE_IRAM);
  Mw(++sp, BITS(mcu.pc, 15, 8), MEM_TYPE_IRAM);
  Mw(MEM_SFR_SP, sp, MEM_TYPE_SFR);
  mcu.pc = entry;
}
