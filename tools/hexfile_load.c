#include "../include/hexfile_load.h"
#include "../include/memory.h"
#include <stdio.h>

dword_t hexdata_read(word_t* buf, int width) {
  dword_t hexdata = 0;
  for (int i = 0; i < width; ++i) {
    char c = buf[i];
    if ((c >= '0') && (c <= '9')) {
      hexdata = (hexdata << 4) | (c - '0');
    } else if ((c >= 'a') && (c <= 'f')) {
      hexdata = (hexdata << 4) | (c - 'a') + 10;
    } else {
      hexdata = (hexdata << 4) | (c - 'A') + 10;
    }
  }

  buf += width;
  return hexdata;
}

word_t* hexfile_load(const char* filename) {
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    printf("open file: %s error.\n", filename);
    return NULL;
  }

  word_t* code = (word_t*)malloc(MEM_CODE_SIZE);
  if (code == NULL) {
    printf("malloc failed.\n");
    return NULL;
  }

  // The format of hexfile likes below
  // -----------------------------------
  // |:| A |   B   | C | ··· D ··· | E |
  // -----------------------------------
  // A: Length  of line, 1 byte;
  // B: Address of data, 2 byte;
  // C: Type of data, 1 byte, 00-data, 01-end of file, 02-extend, 03-start of file...;
  // D: Real data, N byte;
  // E: Check sum, 1 byte;

  char hex_line_buf[HEX_LINE_SIZE];
  while (fgets(hex_line_buf, sizeof(hex_line_buf), file)) {
    word_t* c = hex_line_buf;
    if (*c++ != ':') {
      printf("hexfile: %s format error.", filename);
      fclose(file); 
      return NULL;
    }

    word_t len  = hexdata_read(c, 2);
    addr_t addr = hexdata_read(c, 4);
    word_t type = hexdata_read(c, 2);

    switch (type) {
      case HEX_TYPE_DATA:
        for (word_t index = 0; index < len; ++index) {
          code[addr++] = hexdata_read(c, 2);
        }
        break;
      case HEX_TYPE_EOF:
        fclose(file); return code;
      default:
        fclose(file); free(code);
        return NULL;
    }
  }
  fclose(file);
  return code;
}