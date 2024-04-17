#ifndef __MACRO_H__
#define __MACRO_H__

#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// conmon
#define word_t  uint8_t
#define dword_t uint16_t
#define addr_t  uint16_t
#define iaddr_t uint8_t
#define xaddr_t uint16_t
#define caddr_t uint16_t
#define bool    uint8_t

// memory
#define MEM_CODE_SIZE 65536
#define MEM_IRAM_SIZE 128
#define MEM_XRAM_SIZE 65536
#define MEM_SFR_SIZE  128

#define MEM_BIT_IRAM_START 0x20
#define MEM_BIT_SFR_START	 0x80

#define BITMASK(bits) ((1 << (bits)) - 1)
#define BITS(x, hi, lo) (((x) >> (lo)) & BITMASK((hi) - (lo) + 1))
#define SEXT(x, len) ({ struct { int8_t n : len; } __x = { .n = x }; (uint8_t)__x.n; })

#define panic(msg) do { fprintf(stderr, "Panic: %s\n", msg); exit(EXIT_FAILURE); } while(0)
#define STRLEN(CONST_STR) (sizeof(CONST_STR) - 1)


// test
#define REG_SP		(dword_t)0x1000
#define REG_A 	 	(dword_t)0x1001
#define REG_B		 	(dword_t)0x1002
#define REG_PSW		(dword_t)0x1003
#define REG_PC		(dword_t)0x1004
#define REG_DPTR	(dword_t)0x1005
#define CYCLE		  (dword_t)0x1006
#define REG_R0		(dword_t)0x2000
#define REG_R1		(dword_t)0x2001
#define REG_R2		(dword_t)0x2002
#define REG_R3		(dword_t)0x2003
#define REG_R4		(dword_t)0x2004
#define REG_R5		(dword_t)0x2005
#define REG_R6		(dword_t)0x2006
#define REG_R7		(dword_t)0x2007
#define REG_END		(dword_t)0x2FFF

// macro stringizing
#define str_temp(x) #x
#define str(x) str_temp(x)


// macro concatenation
#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)
#define concat3(x, y, z) concat(concat(x, y), z)
#define concat4(x, y, z, w) concat3(concat(x, y), z, w)
#define concat5(x, y, z, v, w) concat4(concat(x, y), z, v, w)

#define Mr memory_read
#define Mw memory_write
#define Mr2(addr, type) (Mr(addr++, type) << 8) | (Mr(addr++, type))


#endif // !__MACRO_H__