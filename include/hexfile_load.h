#ifndef __HEXFILE_LOAD_H__
#define __HEXFILE_LOAD_H__

#include "macro.h"
#define HEX_LINE_SIZE 1024
#define HEX_TYPE_DATA 0x00
#define HEX_TYPE_EOF  0x01

dword_t hexdata_read(word_t **hex_line_buf, int width);
word_t* hexfile_load(const char* filename);
#endif // !__HEXFILE_LOAD_H__