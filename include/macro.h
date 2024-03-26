#ifndef __MACRO_H__
#define __MACRO_H__

#include <stdint.h> 
// conmon
#define word_t  uint8_t
#define addr_t  uint16_t
#define iaddr_t uint8_t
#define xaddr_t uint16_t
#define caddr_t uint16_t

// memory
#define MEM_CODE_SIZE 65536
#define MEM_IRAM_SIZE 128
#define MEM_XRAM_SIZE 65536
#define MEM_SFR_SIZE  128

#define MEM_BIT_IRAM_START 0x20
#define MEM_BIT_SFR_START	 0x80


#endif // !__MACRO_H__