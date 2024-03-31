#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "macro.h"

#define MEM_CODE_SIZE 65536
#define MEM_IRAM_SIZE 128
#define MEM_XRAM_SIZE 65536
#define MEM_SFR_SIZE  128

#define MEM_BIT_IRAM_START 0x20
#define MEM_BIT_SFR_START	 0x80


#define SFR_ACC       0xE0
#define SFR_SP				0x81
#define SFR_B				  0xF0
#define SFR_DPH				0x83
#define SFR_DPL				0x82
#define SFR_PSW				0xD0
#define SFR_PSW_CY		(0xD0 + 7)
#define SFR_PSW_AC		(0xD0 + 6)
#define SFR_PSW_OV		(0xD0 + 2)
#define SFR_PSW_P			(0xD0 + 0)

#define SFR_SBUF			0x99
#define SFR_SCON			0x98
#define SFR_SCON_TI		(0x98 + 1)
#define SFR_SCON_RI		(0x98 + 0)

#define SFR_TCON			0x88
#define	SFR_TCON_TR0	(0x88 + 4)
#define SFR_TCON_TF0	(0x88 + 5)

#define SFR_TMOD			0x89
#define SFR_TH0				0x8C
#define SFR_TL0				0x8A

#define SFR_IE				0xA8

typedef enum _memory_type_t {
  MEM_TYPE_IRAM,      // not include bit addressable memory
  MEM_TYPE_XRAM,
  MEM_TYPE_CODE,
  MEM_TYPE_BIT,
  MEM_TYPE_SFR,       // not include bit addressable memory
} memory_type_t;

typedef struct _memory_t {
  word_t* code;
  word_t iram[MEM_IRAM_SIZE];
  word_t xram[MEM_XRAM_SIZE];
  word_t sfr[MEM_SFR_SIZE];
} memory_t;

memory_t memory;

void   memory_init(memory_t *memory);

word_t memory_read(addr_t addr, memory_type_t type);
void   memory_write(addr_t addr, word_t data, memory_type_t type);

word_t bit_read (word_t addr);
void   bit_write(word_t addr, word_t bit);

word_t sfr_read (iaddr_t addr);
void   sfr_write(iaddr_t addr, word_t data);

word_t iram_read (iaddr_t addr);
void   iram_write(iaddr_t addr, word_t data);

word_t xram_read (xaddr_t addr);
void   xram_write(xaddr_t addr, word_t data);

word_t code_read (caddr_t addr);
void   code_write(caddr_t addr, word_t data);

#endif // !__MEMORY_H__
