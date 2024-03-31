#include "../include/inst.h"
#include "../include/memory.h"
#include "../include/mcu.h"
#include <stdio.h>
#include <assert.h>

#define Mr memory_read
#define Mw memory_write

// #define Rs()  do { rs = SEXT(BITS(Mr(SFR_PSW, MEM_TYPE_IRAM), 4, 3) << 3, 8); } while(0)
// #define Rn()  do { rn = SEXT(BITS(opcode, 2, 0), 8); } while(0)
// #define Ri()  do { ri = SEXT(BITS(opcode, 0, 0), 8); } while(0)

#define Rs()  do { rs = (Mr(SFR_PSW, MEM_TYPE_IRAM) >> 3) & 0x03 ; } while(0)
#define Rn()  do { rn = opcode & 0x07; } while(0)
#define Ri()  do { ri = opcode & 0x01; } while(0)

void decode_operand(inst_encode_t *inst_encode, word_t *op0, word_t *op1, 
    dword_t bytes, dword_t cycles, op_addr_mode_t op0_mode, op_addr_mode_t op1_mode) {
  word_t opcode = inst_encode->inst_byte0;
  word_t ibyte1 = inst_encode->inst_byte1;
  word_t ibyte2 = inst_encode->inst_byte2;
  word_t rs = 0, rn = 0, ri = 0;
  switch (op0_mode) {
    case OP_ADDR_MODE_REG_RN: Rs(); Rn();  *op0 = rs + rn;                  break;
    case OP_ADDR_MODE_IMM_8 :              *op0 = inst_encode->inst_byte1;  break;
    case OP_ADDR_MODE_DIRECT:              *op0 = inst_encode->inst_byte1;  break;
    case OP_ADDR_MODE_REG_ACC:             *op0 = REG_A;                    break;
    case OP_ADDR_MODE_NONE: break;
  }
  switch (op1_mode) {
    case OP_ADDR_MODE_REG_RN: Rs(); Rn();  *op1 = rs + rn;                  break;
    case OP_ADDR_MODE_IMM_8:               *op1 = inst_encode->inst_byte1;  break;
    case OP_ADDR_MODE_DIRECT:              *op1 = inst_encode->inst_byte1;  break;
    case OP_ADDR_MODE_REG_ACC:             *op1 = REG_A;                    break;
    case OP_ADDR_MODE_NONE: break;
  }
  mcu.pc += bytes; mcu.cycles += cycles;
}

void decode_exec(inst_encode_t *inst_encode) {
  word_t op0, op1; dword_t bytes, cycles;
#define INSTPAT_INST(inst_encode) (inst_encode->inst_byte0)
#define INSTPAT_MATCH(inst_encode, bytes, cycles, name, op0_mode, op1_mode,  ... ) {  \
  decode_operand(inst_encode, &op0, &op1, bytes, cycles, concat(OP_ADDR_MODE_, op0_mode), concat(OP_ADDR_MODE_, op1_mode)); \
  __VA_ARGS__ ; \
}
  INSTPAT_START();
  INSTPAT("0000 0000", 1, 1, NOP , NONE, NONE);
  INSTPAT("1110 1???", 1, 1, MOV A Rn     , REG_ACC,  REG_RN ,  Mw(op0, op1, MEM_TYPE_SFR) );
  INSTPAT("1111 0101", 2, 1, MOV dierct A , DIRECT ,  REG_ACC,  Mw(op0, op1, MEM_TYPE_IRAM));
  INSTPAT("1010 1???", 2, 2, MOV Rn direct, REG_RN ,  DIRECT ,  Mw(op0, op1, MEM_TYPE_IRAM));
  INSTPAT("0111 1???", 2, 1, MOV Rn #data , REG_RN ,  IMM_8  ,  Mw(op0, op1, MEM_TYPE_IRAM));

  INSTPAT("1000 0000", 2, 2, SJMP rel     , DIRECT ,  NONE   ,  mcu.pc += op0);
  INSTPAT_END();
}

void inst_fetch(inst_encode_t* inst_encode, int len) {
  inst_encode->inst_byte0 = Mr(mcu.pc, MEM_TYPE_CODE);
  inst_encode->inst_byte1 = Mr(mcu.pc + 1, MEM_TYPE_CODE);
  inst_encode->inst_byte2 = Mr(mcu.pc + 2, MEM_TYPE_CODE);
}

void inst_exec_once(inst_encode_t* inst_encode) {  
  inst_fetch(inst_encode, 3);
  decode_exec(inst_encode);
}
