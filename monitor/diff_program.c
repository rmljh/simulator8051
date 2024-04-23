#include "../include/inst.h"
#include "../include/memory.h"
#include "../include/hexfile_load.h"
#include <stdio.h>


int difftest() {
  addr_t addr = 0x600;
  do {
    dword_t cmp_addr = Mr2(addr, MEM_TYPE_CODE);
    dword_t exe_data = Mr2(addr, MEM_TYPE_CODE);
    dword_t rel_data;

    switch (cmp_addr) {
      case REG_SP:  rel_data = Mr(MEM_SFR_SP, MEM_TYPE_SFR) ; break;
      case REG_A:   rel_data = Mr(MEM_SFR_ACC, MEM_TYPE_SFR); break;
      case REG_B:   rel_data = Mr(MEM_SFR_B, MEM_TYPE_SFR)  ; break;
      case REG_PSW: rel_data = Mr(MEM_SFR_PSW, MEM_TYPE_SFR); break;  
      case REG_PC:  rel_data = mcu.pc;  break;
      case REG_DPTR:
        rel_data  = Mr(MEM_SFR_DPH, MEM_TYPE_SFR) << 8;
        rel_data |= Mr(MEM_SFR_DPL, MEM_TYPE_SFR);
        break;
      case CYCLE:   rel_data = mcu.cycles; break;
      case REG_R0:  rel_data = Mr(0, MEM_TYPE_IRAM);  break;
      case REG_R1:  rel_data = Mr(1, MEM_TYPE_IRAM);  break;
      case REG_R2:  rel_data = Mr(2, MEM_TYPE_IRAM);  break;
      case REG_R3:  rel_data = Mr(3, MEM_TYPE_IRAM);  break;
      case REG_R4:  rel_data = Mr(4, MEM_TYPE_IRAM);  break;
      case REG_R5:  rel_data = Mr(5, MEM_TYPE_IRAM);  break;
      case REG_R6:  rel_data = Mr(6, MEM_TYPE_IRAM);  break;
      case REG_R7:  rel_data = Mr(7, MEM_TYPE_IRAM);  break;
      case REG_END: return 0;
      default:  rel_data = Mr(cmp_addr, MEM_TYPE_IRAM); break;
    }  

    if (rel_data != exe_data) {
      printf("test error: addr=%x, rel=%x, expect=%x\n", cmp_addr, rel_data, exe_data);
      // return 1;
    }
  } while (1);
}


static const char * file_name[] = {
  // "./difftest/t1_simple/Objects/code.hex",
  // "./difftest/t2_move_0/Objects/code.hex",
  // "./difftest/t2_move_1/Objects/code.hex",
  // "./difftest/t2_move_2/Objects/code.hex",
  // "./difftest/t2_move_3/Objects/code.hex",
  // "./difftest/t3_movc_0/Objects/code.hex",
  // "./difftest/t4_movx_0/Objects/code.hex",  
  // "./difftest/t5_push_pop/Objects/code.hex",
  // "./difftest/t6_xch/Objects/code.hex",
  // "./difftest/t7_anl/Objects/code.hex",
  // "./difftest/t8_orl/Objects/code.hex",
  // "./difftest/t9_xrl/Objects/code.hex",
  // "./difftest/t10_cpl_rr_rl/Objects/code.hex",
  // "./difftest/t11_add_0/Objects/code.hex", 
  // "./difftest/t11_add_1/Objects/code.hex",
  // "./difftest/t12_inc/Objects/code.hex",
  // "./difftest/t13_dec/Objects/code.hex",
  // "./difftest/t14_subb_0/Objects/code.hex",
  // "./difftest/t14_subb_1/Objects/code.hex",
  // "./difftest/t15_mul_div_da/Objects/code.hex",
  // "./difftest/t16_jmp_call_ret/Objects/code.hex",
  // "./difftest/t17_djnz_jz_cjne/Objects/code.hex",
  // "./difftest/t18_bit_jb_jc/Objects/code.hex",
  // "./difftest/t19_serial_0/Objects/code.hex",
  // "./difftest/t19_serial_1/Objects/code.hex",
  // "./difftest/t20_timer/Objects/code.hex",
  "./difftest/t21_int/Objects/code.hex",
  // "./difftest/demo.hex",
};


int main() {
  for (int i = 0; i < sizeof(file_name) / sizeof(const char *); ++i) {
    dword_t pc = -1;
    word_t *code;
    printf("begin test: %s\n", file_name[i]);
    mcu_init(&mcu);
    code = hexfile_load(file_name[i]);
    if (code == NULL) {
      printf("load hex file failed: %s", file_name[i]);
      exit(-1);
    }
    memory.code = code;
    while(1) {
      inst_exec_once(&inst_encode);
      interrupt_controller();
      if (uart_rx_ready()) {
        memory_write(MEM_SFR_SCON_RI, 1, MEM_TYPE_BIT);
      }
    }
  } 
}