#ifndef __INST_H__
#define __INST_H__
#include "macro.h"
#include "mcu.h"
#include <assert.h>

// --- pattern matching mechanism ---
__attribute__((always_inline))
static inline void pattern_decode(const char *str, int len,
    dword_t *key, dword_t *mask, dword_t *shift) {
  dword_t __key = 0, __mask = 0, __shift = 0;
#define macro(i) \
  if ((i) > len + 1) goto finish; \
  else { \
    char c = str[i]; \
    assert(c == '0' || c == '1' || c == '?'); \
    __key  = (__key  << 1) | (c == '1' ? 1 : 0); \
    __mask = (__mask << 1) | (c == '?' ? 0 : 1); \
    __shift = ( c == '?' ? __shift + 1 : 0); \
  }

#define macro2(i)  macro(i);   macro((i) + 1)
#define macro4(i)  macro2(i);  macro2((i) + 2)
#define macro8(i)  macro4(i);  macro4((i) + 5)  // to ignote the space
  macro8(0);
#undef macro
finish:
  *key = __key >> __shift;
  *mask = __mask >> __shift;
  *shift = __shift;
}

// --- pattern matching wrappers for decode ---
#define INSTPAT(pattern, ...) do { \
  dword_t key, mask, shift; \
  pattern_decode(pattern, STRLEN(pattern), &key, &mask, &shift); \
  if (((INSTPAT_INST(inst_encode))>> shift & mask) == key) { \
    INSTPAT_MATCH(inst_encode, ##__VA_ARGS__); \
  } \
} while(0)

#define INSTPAT_START(name) { const void ** __instpat_end = &&concat(__instpat_end_, name);
#define INSTPAT_END(name)   concat(__instpat_end_, name): ; }

typedef struct _inst_encode_t {
  word_t inst_byte0;
  word_t inst_byte1;
  word_t inst_byte2;
} inst_encode_t;

inst_encode_t inst_encode;

typedef struct _inst_decode_t {
  word_t opcode;
  word_t op0;
  word_t op1;
} inst_decode_t;

inst_decode_t inst_decode;

// Addressing modes:  Immediate addressing, Direct addressing, Indirect addressing
// Register addressing, Inherent addressing, Indexed addressing, Bit addressing

typedef enum _op_addr_mode_t {
  OP_ADDR_MODE_NONE,

  OP_ADDR_MODE_IMM_8,
  OP_ADDR_MODE_IMM_H,

  OP_ADDR_MODE_DIRECT,

  OP_ADDR_MODE_IND_RI,
  OP_ADDR_MODE_IND_A_PC,
  OP_ADDR_MODE_IND_A_DP,
  OP_ADDR_MODE_IND_DPTR,
  OP_ADDR_MODE_IND_SP,

  OP_ADDR_MODE_REG_RN,
  OP_ADDR_MODE_REG_ACC,
  OP_ADDR_MODE_REG_BR,
  OP_ADDR_MODE_REG_C,
  OP_ADDR_MODE_REG_DP,

  OP_ADDR_MODE_BIT,
} op_addr_mode_t;

typedef struct _inst_info_t {
  word_t bytes;
  word_t cycles;
  op_addr_mode_t op0_mode;
  op_addr_mode_t op1_mode;
  const char* mnemonic;
} inst_info_t;

void decode_operand(inst_encode_t *inst_encode, dword_t *op0, dword_t *op1, 
  dword_t bytes, dword_t cycles, op_addr_mode_t op0_mode, op_addr_mode_t op1_mode);

void inst_fetch(inst_encode_t *inst_encode, int len);
void decode_exec(inst_encode_t *inst_encode);
void inst_exec_once(inst_encode_t* inst_encode);

#endif // !__INST_H__