#include "../include/memory.h"
#include <string.h>

void memory_init(memory_t *memory) {
  memory->code = (word_t*)0;
  memset(memory->iram, 0, MEM_IRAM_SIZE);
  memset(memory->xram, 0, MEM_XRAM_SIZE);
  memset(memory->sfr,  0, MEM_SFR_SIZE);
}

word_t memory_read(addr_t addr, memory_type_t type) {
  switch (type) {
    case MEM_TYPE_IRAM: return iram_read((iaddr_t)addr);
    case MEM_TYPE_XRAM: return xram_read((xaddr_t)addr);
    case MEM_TYPE_CODE: return code_read((caddr_t)addr);
    case MEM_TYPE_SFR : return sfr_read ((iaddr_t)addr);
    case MEM_TYPE_BIT : return bit_read ((iaddr_t)addr);
    default: return 0;
  }
}

void memory_write(addr_t addr, word_t data, memory_type_t type) {
  switch (type) {
    case MEM_TYPE_IRAM: iram_write((iaddr_t)addr, data); break;
    case MEM_TYPE_XRAM: xram_write((xaddr_t)addr, data); break;
    case MEM_TYPE_CODE: code_write((caddr_t)addr, data); break;
    case MEM_TYPE_SFR : sfr_write ((iaddr_t)addr, data); break;
    case MEM_TYPE_BIT : bit_write ((iaddr_t)addr, data); break;
    default: break;
  }
}


word_t bit_read (word_t addr) {
  word_t bit_index = addr % 8;
  if (addr > MEM_BIT_SFR_START) {
    word_t byte_index = (addr - MEM_BIT_SFR_START) / 8 * 8;
    return memory.sfr[byte_index]  & (1 << bit_index) ? 1 : 0;
  } else {
    word_t byte_index = addr / 8 + MEM_BIT_IRAM_START;
    return memory.iram[byte_index] & (1 << bit_index) ? 1 : 0;
  }
}

void bit_write(word_t addr, word_t bit) {
  word_t bit_index = addr % 8;
  bit &= 0x01;
  if (addr > MEM_BIT_SFR_START) {
    word_t byte_index = (addr - MEM_BIT_SFR_START) / 8 * 8;
    memory.sfr[byte_index] &= ~(1 << bit_index);
    memory.sfr[byte_index] |= bit ? (1 << bit_index) : 0;
  } else {
    word_t byte_index = addr / 8 + MEM_BIT_IRAM_START;
    memory.iram[byte_index] &= ~(1 << bit_index);
    memory.iram[byte_index] |= bit ? (1 << bit_index) : 0;
  }
}

word_t sfr_read(iaddr_t addr) {
  // switch (addr) {
  //   case SFR_SBUF: return uart_read();
  //}
  addr -= MEM_BIT_SFR_START;
  return (addr < MEM_SFR_SIZE) ? memory.sfr[addr] : 0;
}

void sfr_write(iaddr_t addr, word_t data) {
  // switch (addr) {
  //   case SFR_SBUF: uart_write();
  //}
  addr -= MEM_BIT_SFR_START;
  if (addr < MEM_SFR_SIZE) memory.sfr[addr] = data;
}

word_t iram_read(iaddr_t addr) {
  if (addr < MEM_IRAM_SIZE) {
    return memory.iram[addr];
  } else {
    return sfr_read(addr);
  }
}

void iram_write(iaddr_t addr, word_t data) {
  if (addr < MEM_IRAM_SIZE) {
    memory.iram[addr] = data;
  } else {
    sfr_write(addr, data);
  }
}

word_t xram_read(xaddr_t addr) {
  return memory.xram[addr];
}

void xram_write(xaddr_t addr, word_t data) {
  memory.xram[addr] = data;
}

word_t code_read(caddr_t addr) {
  return memory.code[addr];
}

void code_write(caddr_t addr, word_t data) {
  memory.code[addr] = data;
}