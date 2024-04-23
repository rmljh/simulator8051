#include "../include/mcu.h"
#include <conio.h>

mcu_t mcu;

word_t uart_read() {
  return _kbhit() ? _getch() : 0;
}

void uart_write(word_t data) {
  putchar(data);
  // printf("pc = %x\n", mcu.pc);
}

int uart_rx_ready() {
  return _kbhit();
}

void mcu_reset(mcu_t* mcu) {
  mcu_init(mcu);
}

void mcu_init(mcu_t *mcu) {
  memory_init(&memory);
  mcu->mem = &memory;
  mcu->pc  = 0;
  mcu->cycles = 0;
  mcu->in_interrupt = false;
} 

void timer_update(int cycles) {
  if (!Mr(MEM_SFR_TCON_TR0, MEM_TYPE_BIT)) return;
  word_t tmod = Mr(MEM_SFR_TMOD, MEM_TYPE_SFR); 
  int count;

  switch (BITS(tmod, 1, 0)) {
    // mode 00: 16-bit auto-reload timer/counter
    // mode 01: 16-bit timer/counter,
    // mode 02: 8-bit auto-reload timer/counter 
    // mode 03: 16-bit auto-reload timer/counter whose interrupt can not be disabled

    case 0x00:  break;    // Typically no additional operations are required in mode 00.
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
    case 0x02:
      count = (Mr(MEM_SFR_TH0, MEM_TYPE_SFR) << 8) | Mr(MEM_SFR_TL0, MEM_TYPE_SFR);
      count += cycles;
      if (count >= 0x100) {
        Mw(MEM_SFR_TCON_TF0, 1, MEM_TYPE_BIT);
        count = 0;
      } 
      Mw(MEM_SFR_TH0, BITS(count, 15, 8), MEM_TYPE_SFR);
      Mw(MEM_SFR_TL0, BITS(count, 7,  0), MEM_TYPE_SFR);
      break;
    case 0x03: 
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
  // MEM_SFR_IE is the flag of interrupt enable, if is unenable, then return
  // if the program already in interrupt, then return
  if (!BITS(ie, 7, 7))    return;
  if (mcu.in_interrupt)   return;

  // timer0 interrupt
  if(BITS(ie, 1, 1) && Mr(MEM_SFR_TCON_TF0, MEM_TYPE_BIT)) {
    interrupt_entry(INT_TIMER0_ENTRY);
    Mw(MEM_SFR_TCON_TF0, 0, MEM_TYPE_BIT);
  }
}

void interrupt_entry(addr_t entry) {
  word_t sp = Mr(MEM_SFR_SP, MEM_TYPE_SFR);
  mcu.in_interrupt = true;
  Mw(++sp, BITS(mcu.pc, 7,  0), MEM_TYPE_IRAM);
  Mw(++sp, BITS(mcu.pc, 15, 8), MEM_TYPE_IRAM);
  Mw(MEM_SFR_SP, sp, MEM_TYPE_SFR);
  mcu.pc = entry;
}
