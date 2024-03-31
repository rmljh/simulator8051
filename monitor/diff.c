#include "../include/inst.h"
#include "../include/memory.h"
#include "../include/hexfile_load.h"
#include <stdio.h>

void difftest() {
  uint16_t addr = 0x600;
  do {
    uint8_t addr0 = memory_read(addr++, MEM_TYPE_CODE);
    uint8_t addr1 = memory_read(addr++, MEM_TYPE_CODE);
    uint8_t data0 = memory_read(addr++, MEM_TYPE_CODE);
    uint8_t data1 = memory_read(addr++, MEM_TYPE_CODE);
    uint16_t cmp_addr = (addr0 << 8) | addr1;
    uint16_t expect_data = (data0 << 8) | data1;
    uint16_t real_data;

    switch (cmp_addr) {
    case REG_SP:
      real_data = memory_read(SFR_SP, MEM_TYPE_SFR);
      break;
    case REG_A:
      real_data = memory_read(SFR_ACC, MEM_TYPE_SFR);
      break;
    case REG_B:
      real_data = memory_read(SFR_B, MEM_TYPE_SFR);
      break;
    case REG_PSW:
      real_data = memory_read(SFR_PSW, MEM_TYPE_SFR);
      break;  
    case REG_PC:
      real_data = mcu.pc;
      break;
    case REG_DPTR:
      real_data = memory_read(SFR_DPH, MEM_TYPE_SFR) << 8;
      real_data |= memory_read(SFR_DPL, MEM_TYPE_SFR);
      break;
    case CYCLE:
      real_data = mcu.cycles;
      break;
    case REG_R0:
      real_data = memory_read(0, MEM_TYPE_IRAM);
      break;
    case REG_R1:
      real_data = memory_read(1, MEM_TYPE_IRAM);
      break;
    case REG_R2:
      real_data = memory_read(2, MEM_TYPE_IRAM);
      break;
    case REG_R3:
      real_data = memory_read(3, MEM_TYPE_IRAM);
      break;
    case REG_R4:
      real_data = memory_read(4, MEM_TYPE_IRAM);
      break;
    case REG_R5:
      real_data = memory_read(5, MEM_TYPE_IRAM);
      break;
    case REG_R6:
      real_data = memory_read(6, MEM_TYPE_IRAM);
      break;
    case REG_R7:
      real_data = memory_read(7, MEM_TYPE_IRAM);
      break;
    case REG_END:
      return;
    default:
      real_data = memory_read(cmp_addr, MEM_TYPE_IRAM);
      break;
    }  
    if (real_data != expect_data) {
      printf("test error: addr=%x, real=%x, expect=%x\n", cmp_addr, real_data, expect_data);
    } else {
      printf("success!\n");
    }
  } while (1);
}


int main() {
  mcu_init(&mcu);
  memory.code = hexfile_load("./test/code.hex");
  for (int i = 0; i < sizeof(memory.code); ++i) {
    printf("%x ", memory.code[i]);
  }
  // word_t code[] = { 0x78, 0x89, 0xa8, 0x91 };
  // memory_write(0x91, 0x05, MEM_TYPE_IRAM);
  // memory.code = code; 
  int pc;
  while(mcu.pc != sizeof(memory.code)) {
    pc = mcu.pc;
    inst_exec_once(&inst_encode);
    printf("%d\n", mcu.pc);
  } 
  difftest();
}

// int main() {
//   // inst_encode.inst_byte0 = 0x78;
//   // inst_encode.inst_byte1 = 0x89;
//   // inst_encode.inst_byte2 = 0x00;
//   // decode_exec(inst_encode);
//   // word_t data = Mr(0x00, MEM_TYPE_IRAM);
//   // printf("%d", data);
//   word_t code[] = { 0x78, 0x89, 0xa8, 0x91 };
//   Mw(0x91, 0x05, MEM_TYPE_IRAM);
//   memory.code = code; 
//   while (mcu.pc != 4) {
//     inst_fetch(&inst_encode, 3);
//     printf("%d, %d, %d\n", inst_encode.inst_byte0, inst_encode.inst_byte1, inst_encode.inst_byte2);
//     decode_exec(&inst_encode);
//     word_t data = Mr(0x00, MEM_TYPE_IRAM);
//     printf("%d\n", data);
//     printf("%d\n", mcu.pc);
//   }
// }
