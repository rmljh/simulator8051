#include "../include/inst.h"
#include "../include/memory.h"
#include "../include/mcu.h"
#include <stdio.h>
#include <assert.h>

inst_encode_t inst_encode;
inst_decode_t inst_decode;

// #define Rs()  do { rs = SEXT(BITS(Mr(SFR_PSW, MEM_TYPE_IRAM), 4, 3) << 3, 8); } while(0)
// #define Rn()  do { rn = SEXT(BITS(opcode, 2, 0), 8); } while(0)
// #define Ri()  do { ri = SEXT(BITS(opcode, 0, 0), 8); } while(0)

#define Rs()   do { rs = BITS(Mr(MEM_SFR_PSW, MEM_TYPE_IRAM), 4, 3) * 8; } while(0)
#define Rn()   do { rn = BITS(opcode, 2, 0); } while(0)
#define Ri()   do { ri = BITS(opcode, 0, 0); } while(0)
#define DPTR() do { dptr = (Ir(MEM_SFR_DPH) << 8) | Ir(MEM_SFR_DPL); } while(0)

#define Ir(i) Mr(i, MEM_TYPE_IRAM)
#define Sr(i) Mr(i, MEM_TYPE_SFR )
#define Br(i) Mr(i, MEM_TYPE_BIT )
#define Cr(i) Mr(i, MEM_TYPE_CODE)
#define Xr(i) Mr(i, MEM_TYPE_XRAM)

#define SP_INC()  Mw(MEM_SFR_SP, Ir(MEM_SFR_SP) + 1, MEM_TYPE_SFR)
#define SP_SUB()  Mw(MEM_SFR_SP, Ir(MEM_SFR_SP) - 1, MEM_TYPE_SFR)

#define Or0()   do { or0 = Ir(op0); } while(0)
#define Or1()   do { or1 = Ir(op1); } while(0)

#define ADD_PSW_CY() do { word_t flag = Add_Flag_Update(7); Mw(MEM_SFR_PSW_CY, flag, MEM_TYPE_BIT); } while(0)
#define ADD_PSW_AC() do { word_t flag = Add_Flag_Update(3); Mw(MEM_SFR_PSW_AC, flag, MEM_TYPE_BIT); } while(0)
#define ADD_PSW_OV() do { word_t flag = Add_Flag_Update(7) ^ Add_Flag_Update(6); Mw(MEM_SFR_PSW_OV, flag, MEM_TYPE_BIT); } while(0)
#define ADD_FLAG_UPDATE() ADD_PSW_CY(); ADD_PSW_AC(); ADD_PSW_OV()

#define SUB_PSW_CY() do { word_t flag = Sub_Flag_Update(7); Mw(MEM_SFR_PSW_CY, flag, MEM_TYPE_BIT); } while(0)
#define SUB_PSW_AC() do { word_t flag = Sub_Flag_Update(7); Mw(MEM_SFR_PSW_AC, flag, MEM_TYPE_BIT); } while(0)
#define SUB_PSW_OV() do { word_t flag = Sub_Flag_Update(7) ^ Sub_Flag_Update(6); Mw(MEM_SFR_PSW_OV, flag, MEM_TYPE_BIT); } while(0)
#define SUB_FLAG_UPDATE() SUB_PSW_CY(); SUB_PSW_AC(); SUB_PSW_OV()

#define Xchd()  do { word_t lor0 = BITS(or0, 3, 0); word_t lor1 = BITS(or1, 3, 0); \
    or0 = or0 - lor0 + lor1; or1 = or1 - lor1 + lor0;} while(0)

#define Swap()  do { or0 = BITS(or0, 7, 4) | BITS(or0, 3, 0) << 4; } while(0)

#define RL()    do { or0 = BITS(or0, 7, 7) | or0 << 1; } while(0)
#define RR()    do { or0 = BITS(or0, 0, 0) << 7 | BITS(or0, 7, 1); } while(0)
#define RLC()   do { or1 = BITS(or0, 7, 7); or0 = Br(MEM_SFR_PSW_CY)      | (or0 << 1); } while(0)
#define RRC()   do { or1 = BITS(or0, 0, 0); or0 = Br(MEM_SFR_PSW_CY) << 7 | (or0 >> 1); } while(0)

// #define Add()   do { ADD_FLAG_UPDATE(); or0 += or1; } while(0)
// #define Sub()   do { SUB_FLAG_UPDATE(); or0 -= or1; } while(0)


// depend the bit_carry_num(i) to update the flag
#define Add_Flag_Update(i) (BITS(*or0, i, 0) + BITS(*or1, i, 0)) >> (i + 1)

// ADD's opcode: 0010 ????, ADDC's opcode: 0011 ????, ADDC simultaneously 
// adds the byte variable indicated, the Carry flag and the Accumulator, 
// leaving the result in the Accumulator.
// sets the CY, AC, OV if a carry out of bit7, bit3, bit7 ^ bit6.
static void Add(word_t *or0, word_t *or1) {
  // ADDC has to add the Carry flag
  if (BITS(inst_encode.inst_byte0, 4, 4)) {
    *or1 += Mr(MEM_SFR_PSW_CY, MEM_TYPE_BIT);
  }

  word_t bit7_c = Add_Flag_Update(7);
  word_t bit6_c = Add_Flag_Update(6);
  word_t bit3_c = Add_Flag_Update(3);

  Mw(MEM_SFR_PSW_CY, bit7_c, MEM_TYPE_BIT);
  Mw(MEM_SFR_PSW_AC, bit3_c, MEM_TYPE_BIT);
  Mw(MEM_SFR_PSW_OV, bit7_c ^ bit6_c, MEM_TYPE_BIT);

  *or0 += *or1;
}

// depend the bit_borrow_num(i) to update the flag
#define Sub_Flag_Update(i) BITS(*or0, i, 0) < BITS(*or1, i, 0) ? 1 : 0

// SUBB subtracts the indicated variable and the carry flag together
// from the Accumulator, leaving the result in the Accumulator.
// sets the CY, AC, OV if a borrow is needed for bit7, bit3, bit7 ^ bit6
static void Sub(word_t *or0, word_t *or1) {
  *or1 += Mr(MEM_SFR_PSW_CY, MEM_TYPE_BIT);

  word_t bit7_b = Sub_Flag_Update(7);
  word_t bit6_b = Sub_Flag_Update(6);
  word_t bit3_b = Sub_Flag_Update(3);

  Mw(MEM_SFR_PSW_CY, bit7_b, MEM_TYPE_BIT);
  Mw(MEM_SFR_PSW_AC, bit3_b, MEM_TYPE_BIT);
  Mw(MEM_SFR_PSW_OV, bit7_b ^ bit6_b, MEM_TYPE_BIT);

  *or0 -= *or1; 
}

static void Mul(word_t *or0, word_t *or1) { 
  dword_t mul = *or0 * *or1; 
  Mw(MEM_SFR_PSW_OV, (mul > 0xff ? 1 : 0), MEM_TYPE_BIT); 
  Mw(MEM_SFR_PSW_CY, 0, MEM_TYPE_BIT); 
  *or0 = BITS(mul, 7,  0); 
  *or1 = BITS(mul, 15, 8); 
} 

static void Div(word_t *or0, word_t *or1) { 
  if (*or1 == 0) {
    Mw(MEM_SFR_PSW_OV, 1, MEM_TYPE_BIT); 
  } else { 
    word_t div = *or0 / *or1; 
    *or1 = *or0 % *or1; 
    *or0 = div; 
    Mw(MEM_SFR_PSW_OV, 0 , MEM_TYPE_BIT); 
  } 
  Mw(MEM_SFR_PSW_CY, 0 , MEM_TYPE_BIT); 
}

// Decimal-adjust Accumulator for addition
static void Da(word_t *or0) { 
  word_t ac = Mr(MEM_SFR_PSW_AC, MEM_TYPE_BIT); 
  word_t cy = Mr(MEM_SFR_PSW_CY, MEM_TYPE_BIT); 

  // contents of Accumulator are BCD
  dword_t acc = *or0; 
  if (BITS(acc, 3, 0) > 9 || ac == 1)  {
    acc += 0x6;
  }
  // set the CY if a carry out of the low-order four-bit field 
  // propagated through all high-order bits
  if (BITS(acc, 15, 8) != 0) {
    cy = 1;
  } 

  if (BITS(acc, 7, 4) > 9 || cy == 1) {
    acc += 0x60; 
  }

  // set the CY if there was a carry out of the high-order bits
  if (BITS(acc, 15, 8) != 0) {
    cy = 1;
  }
   
  *or0 = (word_t)acc; 
  Mw(MEM_SFR_PSW_CY, cy, MEM_TYPE_BIT); 
}

static void Call(inst_encode_t *inst_encode, char Type) {
  word_t sp = Mr(MEM_SFR_SP, MEM_TYPE_SFR) + 1;
  Mw(sp++, BITS(mcu.pc, 7,  0), MEM_TYPE_IRAM);
  Mw(sp  , BITS(mcu.pc, 15, 8), MEM_TYPE_IRAM);
  Mw(MEM_SFR_SP, sp, MEM_TYPE_SFR);
  switch (Type) {
    case 'A': mcu.pc = (mcu.pc & 0xf800) | ((inst_encode->inst_byte0 & 0xe0) << 3) | inst_encode->inst_byte1; break;
    case 'L': mcu.pc = (inst_encode->inst_byte1 << 8) | inst_encode->inst_byte2;                              break;
  }
}

static void Ret() { 
  word_t sp = Sr(MEM_SFR_SP); 
  mcu.pc = Ir(sp--) << 8 | Ir(sp--); 
  Mw(MEM_SFR_SP, sp, MEM_TYPE_SFR); 
}

static void psw_p_update()  { 
  // word_t count = 0; word_t acc = Ir(MEM_SFR_ACC); 
  // for (word_t i = 0; i < 8; ++i) { 
  //   if (BITS(acc, i, i) == 1) count ^= 1; 
  // } 
  // Mw(MEM_SFR_PSW_P, count, MEM_TYPE_BIT); 
  word_t v = Ir(MEM_SFR_ACC);
  v = (v >> 4) ^ (v & 0xF);
  v = (v >> 2) ^ (v & 0x3);
  v = (v >> 1) ^ (v & 0x1);
  Mw(MEM_SFR_PSW_P, v, MEM_TYPE_BIT);
} 


void decode_operand(inst_encode_t *inst_encode, dword_t *op0, dword_t *op1, 
    dword_t bytes, dword_t cycles, op_addr_mode_t op0_mode, op_addr_mode_t op1_mode) {
  word_t opcode = inst_encode->inst_byte0;
  word_t ibyte1 = inst_encode->inst_byte1;
  word_t ibyte2 = inst_encode->inst_byte2;

  inst_decode.opcode = opcode;

  word_t rs = 0, rn = 0, ri = 0;
  dword_t dptr = 0;

  switch (op0_mode) {
    case OP_ADDR_MODE_REG_RN:   Rs(); Rn(); *op0 = rs + rn;                      inst_decode.or0 = Ir(*op0);  break;
    case OP_ADDR_MODE_REG_ACC:              *op0 = MEM_SFR_ACC;                  inst_decode.or0 = Ir(*op0);  break;
    case OP_ADDR_MODE_REG_C :               *op0 = MEM_SFR_PSW_CY;               inst_decode.or0 = Br(*op0);  break;
    case OP_ADDR_MODE_REG_DP:               *op0 = MEM_SFR_DPH;                  inst_decode.or0 = Ir(*op0);  break;
    case OP_ADDR_MODE_IMM_8 :               *op0 = inst_encode->inst_byte1;      inst_decode.or0 = *op0;      break;
    case OP_ADDR_MODE_IMM_H :               *op0 = inst_encode->inst_byte1;      inst_decode.or0 = *op0;      break;
    case OP_ADDR_MODE_DIRECT:               *op0 = inst_encode->inst_byte1;      inst_decode.or0 = Ir(*op0);      break;
    case OP_ADDR_MODE_IND_RI:   Rs(); Ri(); *op0 = Mr(rs + ri, MEM_TYPE_IRAM);   inst_decode.or0 = Ir(*op0);  break;
    case OP_ADDR_MODE_IND_A_DP: DPTR();     *op0 = Ir(MEM_SFR_ACC) + dptr ;      inst_decode.or0 = Ir(*op0);  break;
    case OP_ADDR_MODE_IND_A_PC:             *op0 = Ir(MEM_SFR_ACC) + mcu.pc + 1; inst_decode.or0 = Ir(*op0);  break;
    case OP_ADDR_MODE_IND_DPTR: DPTR();     *op0 = dptr;                         inst_decode.or0 = Ir(*op0);  break;
    case OP_ADDR_MODE_BIT :                 *op0 = inst_encode->inst_byte1;      inst_decode.or0 = Br(*op0);  break;
    case OP_ADDR_MODE_NONE: break;
  }

  switch (op1_mode) {
    case OP_ADDR_MODE_REG_RN:   Rs(); Rn(); *op1 = rs + rn;                      inst_decode.or1 = Ir(*op1);  break;  
    case OP_ADDR_MODE_REG_ACC:              *op1 = Ir(MEM_SFR_ACC);              inst_decode.or1 = Ir(*op1);  break;
    case OP_ADDR_MODE_REG_C :               *op1 = MEM_SFR_PSW_CY;               inst_decode.or1 = Br(*op1);  break;
    case OP_ADDR_MODE_REG_BR:               *op1 = MEM_SFR_B;                    inst_decode.or1 = Ir(*op1);  break;
    case OP_ADDR_MODE_REG_DP:               *op1 = MEM_SFR_DPL;                  inst_decode.or1 = Ir(*op1);  break;
    case OP_ADDR_MODE_IMM_8:                *op1 = bytes == 3 ? ibyte2 : ibyte1; inst_decode.or1 = *op1;      break;
    case OP_ADDR_MODE_IMM_H :               *op1 = inst_encode->inst_byte2;      inst_decode.or1 = *op1;      break;
    case OP_ADDR_MODE_DIRECT:               *op1 = bytes == 3 ? ibyte2 : ibyte1; inst_decode.or1 = Ir(*op1);  break;
    case OP_ADDR_MODE_IND_RI:   Rs(); Ri(); *op1 = Mr(rs + ri, MEM_TYPE_IRAM);   inst_decode.or1 = Ir(*op1);  break; 
    case OP_ADDR_MODE_IND_A_DP: DPTR();     *op1 = Ir(MEM_SFR_ACC) + dptr ;      inst_decode.or1 = *op1;      break;
    case OP_ADDR_MODE_IND_A_PC:             *op1 = Ir(MEM_SFR_ACC) + mcu.pc + 1; inst_decode.or1 = Ir(*op1);  break;
    case OP_ADDR_MODE_IND_DPTR: DPTR();     *op1 = dptr;                         inst_decode.or1 = Ir(*op1);  break;   
    case OP_ADDR_MODE_BIT :                 *op1 = bytes == 3 ? ibyte2 : ibyte1; inst_decode.or1 = Br(*op1);  break;
    case OP_ADDR_MODE_NONE: break;
  }
  // printf("op0 = %x, op1 = %x\n", *op0, *op1);
  mcu.pc += bytes; mcu.cycles += cycles;
}

void decode_exec(inst_encode_t *inst_encode) {
  dword_t op0, op1; dword_t bytes, cycles; word_t or0, or1;
  word_t opcode = inst_encode->inst_byte0;
  word_t ibyte1 = inst_encode->inst_byte1;
  word_t ibyte2 = inst_encode->inst_byte2;
#define INSTPAT_INST(inst_encode) (inst_encode->inst_byte0)
#define INSTPAT_MATCH(inst_encode, bytes, cycles, name, op0_mode, op1_mode,  ... ) {  \
  decode_operand(inst_encode, &op0, &op1, bytes, cycles, concat(OP_ADDR_MODE_, op0_mode), concat(OP_ADDR_MODE_, op1_mode)); \
  psw_p_update(); timer_update(cycles); \
  __VA_ARGS__ ; \
}
  INSTPAT_START();
  //  ---   pattern -- by cy    ----   name   -----   op0_mode - op1_mode    -----    exec    ------       
  INSTPAT("0000 0000", 1, 1, NOP                    , NONE    , NONE                                     );
  INSTPAT("0111 0100", 2, 1, MOV  A       #data     , REG_ACC , IMM_8   , Mw(op0, op1    , MEM_TYPE_IRAM));
  INSTPAT("0111 0101", 3, 2, MOV  direct  #data     , DIRECT  , IMM_8   , Mw(op0, op1    , MEM_TYPE_IRAM));
  INSTPAT("0111 011?", 2, 1, MOV  @Ri     #data     , IND_RI  , IMM_8   , Mw(op0, op1    , MEM_TYPE_IRAM));
  INSTPAT("0111 1???", 2, 1, MOV  Rn      #data     , REG_RN  , IMM_8   , Mw(op0, op1    , MEM_TYPE_IRAM)); 
  INSTPAT("1000 0011", 1, 2, MOVC A       @A + PC   , REG_ACC , IND_A_PC, Mw(op0, Cr(op1), MEM_TYPE_IRAM));
  INSTPAT("1000 0101", 3, 2, MOV  direct  direct    , DIRECT  , DIRECT  , Mw(op1, Ir(op0), MEM_TYPE_IRAM));
  INSTPAT("1000 011?", 2, 2, MOV  direct  @Ri       , DIRECT  , IND_RI  , Mw(op0, Ir(op1), MEM_TYPE_IRAM));
  INSTPAT("1000 1???", 2, 2, MOV  direct  Rn        , DIRECT  , REG_RN  , Mw(op0, Ir(op1), MEM_TYPE_IRAM));
  INSTPAT("1001 0000", 3, 2, MOV  DPTR    #data 16  , IMM_H   , IMM_H   , Mw(MEM_SFR_DPH, op0, 0); Mw(MEM_SFR_DPL, op1, 0));
  INSTPAT("1001 0010", 2, 2, MOV  bit     C         , BIT     , REG_C   , Mw(op0, Br(op1), MEM_TYPE_BIT ));
  INSTPAT("1001 0011", 1, 2, MOVC A       @A + DPTR , REG_ACC , IND_A_DP, Mw(op0, Cr(op1), MEM_TYPE_IRAM));
  INSTPAT("1010 0010", 2, 1, MOV  C       bit       , REG_C   , BIT     , Mw(op0, Br(op1), MEM_TYPE_BIT ));
  INSTPAT("1010 011?", 2, 2, MOV  @Ri     direct    , IND_RI  , DIRECT  , Mw(op0, Ir(op1), MEM_TYPE_IRAM));
  INSTPAT("1010 1???", 2, 2, MOV  Rn      direct    , REG_RN  , DIRECT  , Mw(op0, Ir(op1), MEM_TYPE_IRAM)); 
  INSTPAT("1110 0101", 2, 1, MOV  A       direct    , REG_ACC , DIRECT  , Mw(op0, Ir(op1), MEM_TYPE_IRAM));
  INSTPAT("1110 011?", 1, 1, MOV  A       @Rn       , REG_ACC , IND_RI  , Mw(op0, Ir(op1), MEM_TYPE_IRAM));
  INSTPAT("1110 1???", 1, 1, MOV  A       Rn        , REG_ACC , REG_RN  , Mw(op0, Ir(op1), MEM_TYPE_IRAM));
  INSTPAT("1111 0101", 2, 1, MOV  dierct  A         , DIRECT  , REG_ACC , Mw(op0, op1    , MEM_TYPE_IRAM));
  INSTPAT("1111 011?", 1, 1, MOV  @Ri     A         , IND_RI  , REG_ACC , Mw(op0, op1    , MEM_TYPE_IRAM));
  INSTPAT("1111 1???", 1, 1, MOV  Rn      A         , REG_RN  , REG_ACC , Mw(op0, op1    , MEM_TYPE_IRAM));

  INSTPAT("1110 001?", 1, 2, MOVX A       @Ri       , REG_ACC , IND_RI  , Mw(op0, Xr(op1), MEM_TYPE_IRAM));    // 暂时还未校验
  INSTPAT("1111 001?", 1, 2, MOVX @Ri     A         , IND_RI  , REG_ACC , Mw(op0, op1    , MEM_TYPE_XRAM));    // 暂时还未校验
  INSTPAT("1111 0000", 1, 2, MOVX @DPTR   A         , IND_DPTR, REG_ACC , Mw(op0, op1    , MEM_TYPE_XRAM));    // 暂时还未校验
  INSTPAT("1110 0000", 1, 2, MOVX A       @DPTR     , REG_ACC , IND_DPTR, Mw(op0, Xr(op1), MEM_TYPE_IRAM));    // 暂时还未校验

  INSTPAT("1100 0000", 2, 2, PUSH direct            , DIRECT  , NONE    , SP_INC(); Mw(Ir(MEM_SFR_SP), Ir(op0), MEM_TYPE_IRAM));
  INSTPAT("1101 0000", 2, 2, POP  direct            , DIRECT  , NONE    , Mw(op0, Ir(Ir(MEM_SFR_SP)), MEM_TYPE_IRAM); SP_SUB());

  INSTPAT("1100 1???", 1, 1, XCH  A       Rn        , REG_ACC , REG_RN  , Or0(); Or1(); Mw(op0, or1, MEM_TYPE_IRAM); Mw(op1, or0, MEM_TYPE_IRAM));
  INSTPAT("1100 0101", 2, 1, XCH  A       direct    , REG_ACC , DIRECT  , Or0(); Or1(); Mw(op0, or1, MEM_TYPE_IRAM); Mw(op1, or0, MEM_TYPE_IRAM));
  INSTPAT("1100 011?", 1, 1, XCH  A       @Ri       , REG_ACC , IND_RI  , Or0(); Or1(); Mw(op0, or1, MEM_TYPE_IRAM); Mw(op1, or0, MEM_TYPE_IRAM));
  INSTPAT("1101 011?", 1, 1, XCHD A       @Ri       , REG_ACC , IND_RI  , Or0(); Or1(); Xchd(); Mw(op0, or0, MEM_TYPE_IRAM); Mw(op1, or1, MEM_TYPE_IRAM));
  INSTPAT("1100 0100", 1, 1, SWAP A                 , REG_ACC , NONE    , Or0(); Swap(); Mw(op0, or0, MEM_TYPE_IRAM));
  
  INSTPAT("0101 1???", 1, 1, ANL  A       @Rn       , REG_ACC , REG_RN  , Mw(op0, (Ir(op0) & Ir(op1)), MEM_TYPE_IRAM));
  INSTPAT("0101 0101", 2, 1, ANL  A       direct    , REG_ACC , DIRECT  , Mw(op0, (Ir(op0) & Ir(op1)), MEM_TYPE_IRAM));
  INSTPAT("0101 011?", 1, 1, ANL  A       @Ri       , REG_ACC , IND_RI  , Mw(op0, (Ir(op0) & Ir(op1)), MEM_TYPE_IRAM));
  INSTPAT("0101 0100", 2, 1, ANL  A       #data     , REG_ACC , IMM_8   , Mw(op0, (Ir(op0) & op1)    , MEM_TYPE_IRAM));
  INSTPAT("0101 0010", 2, 1, ANL  direct  A         , DIRECT  , REG_ACC , Mw(op0, (Ir(op0) & op1)    , MEM_TYPE_IRAM));
  INSTPAT("0101 0011", 3, 2, ANL  direct  #data     , DIRECT  , IMM_8   , Mw(op0, (Ir(op0) & op1)    , MEM_TYPE_IRAM));
  INSTPAT("1000 0010", 2, 2, ANL  C       bit       , REG_C   , BIT     , Mw(op0, (Br(op0) &  Br(op1)), MEM_TYPE_BIT));
  INSTPAT("1011 0000", 2, 2, ANL  C       ~bit      , REG_C   , BIT     , Mw(op0, (Br(op0) & !Br(op1)), MEM_TYPE_BIT));

  INSTPAT("0100 1???", 1, 1, ORL  A       Rn        , REG_ACC , REG_RN  , Mw(op0, (Ir(op0) | Ir(op1)), MEM_TYPE_IRAM));
  INSTPAT("0100 0101", 2, 1, ORL  A       direct    , REG_ACC , DIRECT  , Mw(op0, (Ir(op0) | Ir(op1)), MEM_TYPE_IRAM));
  INSTPAT("0100 011?", 1, 1, ORL  A       @Ri       , REG_ACC , IND_RI  , Mw(op0, (Ir(op0) | Ir(op1)), MEM_TYPE_IRAM));
  INSTPAT("0100 0100", 2, 1, ORL  A       #data     , REG_ACC , IMM_8   , Mw(op0, (Ir(op0) | op1)    , MEM_TYPE_IRAM));
  INSTPAT("0100 0010", 2, 1, ORL  direct  A         , DIRECT  , REG_ACC , Mw(op0, (Ir(op0) | op1)    , MEM_TYPE_IRAM));
  INSTPAT("0100 0011", 3, 2, ORL  direct  #data     , DIRECT  , IMM_8   , Mw(op0, (Ir(op0) | op1)    , MEM_TYPE_IRAM));
  INSTPAT("0111 0010", 2, 2, ORL  C       bit       , REG_C   , BIT     , Mw(op0, (Br(op0) |  Br(op1)), MEM_TYPE_BIT));
  INSTPAT("1010 0000", 2, 2, ORL  C       ~bit      , REG_C   , BIT     , Mw(op0, (Br(op0) | !Br(op1)), MEM_TYPE_BIT));

  INSTPAT("0110 1???", 1, 1, XRL  A       Rn        , REG_ACC , REG_RN  , Mw(op0, (Ir(op0) ^ Ir(op1)), MEM_TYPE_IRAM));
  INSTPAT("0110 0101", 2, 1, XRL  A       direct    , REG_ACC , DIRECT  , Mw(op0, (Ir(op0) ^ Ir(op1)), MEM_TYPE_IRAM));
  INSTPAT("0110 011?", 1, 1, XRL  A       @Ri       , REG_ACC , IND_RI  , Mw(op0, (Ir(op0) ^ Ir(op1)), MEM_TYPE_IRAM));
  INSTPAT("0110 0100", 2, 1, XRL  A       #data     , REG_ACC , IMM_8   , Mw(op0, (Ir(op0) ^ op1)    , MEM_TYPE_IRAM));
  INSTPAT("0110 0010", 2, 1, XRL  direct  A         , DIRECT  , REG_ACC , Mw(op0, (Ir(op0) ^ op1)    , MEM_TYPE_IRAM));
  INSTPAT("0110 0011", 3, 2, XRL  direct  #data     , DIRECT  , IMM_8   , Mw(op0, (Ir(op0) ^ op1)    , MEM_TYPE_IRAM));

  INSTPAT("1110 0100", 1, 1, CLR  A                 , REG_ACC , NONE    , Mw(op0, 0, MEM_TYPE_IRAM));
  INSTPAT("1100 0011", 1, 1, CLR  C                 , REG_C   , NONE    , Mw(op0, 0, MEM_TYPE_BIT ));
  INSTPAT("1100 0010", 2, 1, CLR  bit               , BIT     , NONE    , Mw(op0, 0, MEM_TYPE_BIT ));

  INSTPAT("1111 0100", 1, 1, CPL  A                 , REG_ACC , NONE    , Mw(op0, ~Ir(op0), MEM_TYPE_IRAM));
  INSTPAT("1011 0011", 1, 1, CPL  C                 , REG_C   , NONE    , Mw(op0, ~Br(op0), MEM_TYPE_BIT ));
  INSTPAT("1011 0010", 2, 1, CPL  bit               , BIT     , NONE    , Mw(op0, ~Br(op0), MEM_TYPE_BIT ));

  INSTPAT("0010 0011", 1, 1, RL   A                 , REG_ACC , NONE    , Or0(); RL() ; Mw(op0, or0, MEM_TYPE_IRAM));
  INSTPAT("0011 0011", 1, 1, RLC  A                 , REG_ACC , NONE    , Or0(); RLC(); Mw(op0, or0, MEM_TYPE_IRAM)); 
  INSTPAT("0000 0011", 1, 1, RR   A                 , REG_ACC , NONE    , Or0(); RR() ; Mw(op0, or0, MEM_TYPE_IRAM));
  INSTPAT("0001 0011", 1, 1, RRC  A                 , REG_ACC , NONE    , Or0(); RRC(); Mw(op0, or0, MEM_TYPE_IRAM));

  INSTPAT("0010 1???", 1, 1, ADD  A       Rn        , REG_ACC , REG_RN  , Or0(); Or1(); Add(&or0, &or1); Mw(op0, or0, MEM_TYPE_IRAM));
  INSTPAT("0010 0101", 2, 1, ADD  A       direct    , REG_ACC , DIRECT  , Or0(); Or1(); Add(&or0, &or1); Mw(op0, or0, MEM_TYPE_IRAM));
  INSTPAT("0010 011?", 1, 1, ADD  A       @Ri       , REG_ACC , IND_RI  , Or0(); Or1(); Add(&or0, &or1); Mw(op0, or0, MEM_TYPE_IRAM));
  INSTPAT("0010 0100", 2, 1, ADD  A       #data     , REG_ACC , IMM_8   , Or0(); or1 = op1; Add(&or0, &or1); Mw(op0, or0, MEM_TYPE_IRAM));

  INSTPAT("0011 1???", 1, 1, ADDC A       Rn        , REG_ACC , REG_RN  , Or0(); Or1(); Add(&or0, &or1); Mw(op0, or0, MEM_TYPE_IRAM));
  INSTPAT("0011 0101", 2, 1, ADDC A       direct    , REG_ACC , DIRECT  , Or0(); Or1(); Add(&or0, &or1); Mw(op0, or0, MEM_TYPE_IRAM));
  INSTPAT("0011 011?", 1, 1, ADDC A       @Ri       , REG_ACC , IND_RI  , Or0(); Or1(); Add(&or0, &or1); Mw(op0, or0, MEM_TYPE_IRAM));
  INSTPAT("0011 0100", 2, 1, ADDC A       #data     , REG_ACC , IMM_8   , Or0(); or1 = op1; Add(&or0, &or1); Mw(op0, or0, MEM_TYPE_IRAM));

  INSTPAT("0000 0100", 1, 1, INC  A                 , REG_ACC , NONE    , Or0(); Mw(op0, or0 + 1, MEM_TYPE_IRAM));
  INSTPAT("0000 1???", 1, 1, INC  Rn                , REG_RN  , NONE    , Or0(); Mw(op0, or0 + 1, MEM_TYPE_IRAM));
  INSTPAT("0000 0101", 2, 1, INC  direct            , DIRECT  , NONE    , Or0(); Mw(op0, or0 + 1, MEM_TYPE_IRAM));
  INSTPAT("0000 011?", 1, 1, INC  @Ri               , IND_RI  , NONE    , Or0(); Mw(op0, or0 + 1, MEM_TYPE_IRAM));
  INSTPAT("1010 0011", 1, 2, INC  DPTR              , REG_DP  , REG_DP  , Or0(); Or1(); Mw(op0, (or1 == 0xff ? or0 + 1 : or0), MEM_TYPE_IRAM); Mw(op1, or1 + 1, MEM_TYPE_IRAM));

  INSTPAT("0001 0100", 1, 1, DEC  A                 , REG_ACC , NONE    , Or0(); Mw(op0, or0 - 1, MEM_TYPE_IRAM));
  INSTPAT("0001 1???", 1, 1, DEC  Rn                , REG_RN  , NONE    , Or0(); Mw(op0, or0 - 1, MEM_TYPE_IRAM));
  INSTPAT("0001 0101", 2, 1, DEC  direct            , DIRECT  , NONE    , Or0(); Mw(op0, or0 - 1, MEM_TYPE_IRAM));
  INSTPAT("0001 011?", 1, 1, DEC  @Ri               , IND_RI  , NONE    , Or0(); Mw(op0, or0 - 1, MEM_TYPE_IRAM));

  INSTPAT("1001 1???", 1, 1, SUBB A       Rn        , REG_ACC , REG_RN  , Or0(); Or1(); Sub(&or0, &or1); Mw(op0, or0, MEM_TYPE_IRAM));
  INSTPAT("1001 0101", 2, 1, SUBB A       direct    , REG_ACC , DIRECT  , Or0(); Or1(); Sub(&or0, &or1); Mw(op0, or0, MEM_TYPE_IRAM));
  INSTPAT("1001 011?", 1, 1, SUBB A       @Ri       , REG_ACC , IND_RI  , Or0(); Or1(); Sub(&or0, &or1); Mw(op0, or0, MEM_TYPE_IRAM));
  INSTPAT("1001 0100", 2, 1, SUBB A       #data     , REG_ACC , IMM_8   , Or0(); or1 = op1; Sub(&or0, &or1); Mw(op0, or0, MEM_TYPE_IRAM));

  INSTPAT("1010 0100", 1, 4, MUL  AB                , REG_ACC , REG_BR  , Or0(); Or1(); Mul(&or0, &or1); Mw(op0, or0, MEM_TYPE_IRAM); Mw(op1, or1, MEM_TYPE_IRAM));
  INSTPAT("1000 0100", 1, 4, DIV  AB                , REG_ACC , REG_BR  , Or0(); Or1(); Div(&or0, &or1); Mw(op0, or0, MEM_TYPE_IRAM); Mw(op1, or1, MEM_TYPE_IRAM));
  INSTPAT("1101 0100", 1, 1, DA   A                 , REG_ACC , NONE    , Or0();        Da(&or0);        Mw(op0, or0, MEM_TYPE_IRAM));

  INSTPAT("???1 0001", 2, 2, ACALL  addr11          , NONE    , NONE    , Call(inst_encode, 'A'));
  INSTPAT("0001 0010", 3, 2, LCALL  addr16          , NONE    , NONE    , Call(inst_encode, 'L'));
  INSTPAT("0010 0010", 1, 2, RET                    , NONE    , NONE    , Ret());
  INSTPAT("0011 0010", 1, 2, RETI                   , NONE    , NONE    , Ret(); mcu.in_interrupt = 0);

  INSTPAT("???0 0001", 2, 2, AJMP addr11            , NONE    , NONE    , mcu.pc = BITS(mcu.pc, 15, 11) << 11 | (BITS(opcode, 7, 5) << 8) | ibyte1);
  INSTPAT("1000 0000", 2, 2, SJMP rel               , NONE    , NONE    , mcu.pc += (int8_t)inst_encode->inst_byte1);
  INSTPAT("0000 0010", 3, 2, LJMP addr16            , NONE    , NONE    , mcu.pc = inst_encode->inst_byte1 << 8 | inst_encode->inst_byte2);
  INSTPAT("0111 0011", 1, 2, JMP  @A + DPTR         , NONE    , IND_A_DP, mcu.pc = op1);
  
  INSTPAT("0110 0000", 2, 2, JZ   rel               , NONE    , NONE    , mcu.pc += Ir(MEM_SFR_ACC) == 0 ? (int8_t)ibyte1 : 0);
  INSTPAT("0111 0000", 2, 2, JNZ  rel               , NONE    , NONE    , mcu.pc += Ir(MEM_SFR_ACC) != 0 ? (int8_t)ibyte1 : 0);
  INSTPAT("1101 1???", 2, 2, DJNZ Rn      rel       , REG_RN  , NONE    , Or0(); Mw(op0, --or0, MEM_TYPE_IRAM); mcu.pc += or0 != 0 ? (int8_t)ibyte1 : 0);
  INSTPAT("1101 0101", 3, 2, DJNZ direct  rel       , DIRECT  , NONE    , Or0(); Mw(op0, --or0, MEM_TYPE_IRAM); mcu.pc += or0 != 0 ? (int8_t)ibyte2 : 0);
 
  INSTPAT("1011 0101", 3, 2, CJNE A    direct   rel , REG_ACC , DIRECT  , Or0(); or1 = Ir(ibyte1); mcu.pc += or0 != or1 ? (int8_t)ibyte2 : 0; Mw(MEM_SFR_PSW_CY, (or0 < or1 ? 1 : 0), MEM_TYPE_BIT));
  INSTPAT("1011 0100", 3, 2, CJNE A    #data    rel , REG_ACC , IMM_8   , Or0(); or1 = ibyte1; mcu.pc += or0 != or1 ? (int8_t)ibyte2 : 0; Mw(MEM_SFR_PSW_CY, (or0 < or1 ? 1 : 0), MEM_TYPE_BIT));
  INSTPAT("1011 1???", 3, 2, CJNE Rn   #data    rel , REG_RN  , IMM_8   , Or0(); or1 = ibyte1; mcu.pc += or0 != or1 ? (int8_t)ibyte2 : 0; Mw(MEM_SFR_PSW_CY, (or0 < or1 ? 1 : 0), MEM_TYPE_BIT));
  INSTPAT("1011 011?", 3, 2, CJNE @Ri  #data    rel , IND_RI  , IMM_8   , Or0(); or1 = ibyte1; mcu.pc += or0 != or1 ? (int8_t)ibyte2 : 0; Mw(MEM_SFR_PSW_CY, (or0 < or1 ? 1 : 0), MEM_TYPE_BIT));

  INSTPAT("0010 0000", 3, 2, JB   bit     rel       , BIT     , NONE    , mcu.pc += Br(op0) == 1 ? (int8_t)ibyte2 : 0);
  INSTPAT("0011 0000", 3, 2, JNB  bit     rel       , BIT     , NONE    , mcu.pc += Br(op0) == 0 ? (int8_t)ibyte2 : 0);
  INSTPAT("0001 0000", 3, 2, JBC  bit     rel       , BIT     , NONE    , mcu.pc += Br(op0) == 1 ? (int8_t)ibyte2 : 0; Mw(op0, 0, MEM_TYPE_BIT));
  INSTPAT("0100 0000", 2, 2, JC   rel               , REG_C   , NONE    , mcu.pc += Br(op0) == 1 ? (int8_t)ibyte1 : 0);
  INSTPAT("0101 0000", 2, 2, JNC  rel               , REG_C   , NONE    , mcu.pc += Br(op0) == 0 ? (int8_t)ibyte1 : 0);

  INSTPAT("1101 0011", 1, 1, SETB C                 , REG_C   , NONE    , Mw(op0, 1, MEM_TYPE_BIT));
  INSTPAT("1101 0010", 2, 1, SETB bit               , BIT     , NONE    , Mw(op0, 1, MEM_TYPE_BIT));


  INSTPAT_END();
}

void inst_fetch(inst_encode_t* inst_encode, int len) {
  inst_encode->inst_byte0 = Mr(mcu.pc    , MEM_TYPE_CODE);
  inst_encode->inst_byte1 = Mr(mcu.pc + 1, MEM_TYPE_CODE);
  inst_encode->inst_byte2 = Mr(mcu.pc + 2, MEM_TYPE_CODE);
}

void inst_exec_once(inst_encode_t* inst_encode) {  
  inst_fetch(inst_encode, 3);
  decode_exec(inst_encode);
}

// int main() {
//   word_t code[] = { 0x90, 0x00, 0x0F, 0x74, 0x01, 0x93, 0xF5, 0x20, 0x74, 0x02, 0x83, 0xF5, 0x21, 0x80, 0xFE, 0x12, 0x34 };
//   dword_t pc = -1;
//   mcu_init(&mcu);
//   memory.code = code;
//   while (mcu.pc != sizeof(code) && pc != mcu.pc) {
//     pc = mcu.pc;
//     inst_exec_once(&inst_encode);
//   }
//   word_t data = Mr(0x21, MEM_TYPE_IRAM);
//   printf("%x\n", data);
// }
