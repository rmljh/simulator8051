#include <gtk/gtk.h>
#include "../include/inst.h"
#include "../include/memory.h"
#include "../include/hexfile_load.h"

GtkTextBuffer *global_sfr_buffer = NULL;

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
  // "./difftest/t21_int/Objects/code.hex",
  // "./difftest/demo.hex",
  "./difftest/demo3.hex",
};

static void iram_show(GtkTextBuffer *iram_buffer) {
  word_t count = 0; // 用于计数每行显示的数据数量
  for (int i = 0; i < MEM_IRAM_SIZE; ++i) {
    char hex_string[10];
    if (count % 8 == 0) { // 每行显示8个数据
      gtk_text_buffer_insert_at_cursor(iram_buffer, "\n", -1);
      sprintf(hex_string, "Addr %02X:\t", count);
      gtk_text_buffer_insert_at_cursor(iram_buffer, hex_string, -1);
    }
    
    sprintf(hex_string, "%02X ", memory.iram[i]); // 每个数据占两个字符位置，添加空格分隔
    gtk_text_buffer_insert_at_cursor(iram_buffer, hex_string, -1);
    count++;
  }
}

void sfr_show(GtkTextBuffer *sfr_buffer) {
  int rs = BITS(Mr(MEM_SFR_PSW, MEM_TYPE_IRAM), 4, 3) << 3;
  char hex_string[20];
  gtk_text_buffer_insert_at_cursor(sfr_buffer, "\n", -1);
  for (int i = 0; i < 8; ++i) {
    sprintf(hex_string, "r%d\t\t= %02X\n", i, memory_read(rs + i, MEM_TYPE_IRAM));
    gtk_text_buffer_insert_at_cursor(sfr_buffer, hex_string, -1);
  }
  sprintf(hex_string, "acc\t= %02X\n", memory_read(MEM_SFR_ACC, MEM_TYPE_IRAM));
  gtk_text_buffer_insert_at_cursor(sfr_buffer, hex_string, -1);
  sprintf(hex_string, "b\t\t= %02X\n", memory_read(MEM_SFR_B, MEM_TYPE_IRAM));
  gtk_text_buffer_insert_at_cursor(sfr_buffer, hex_string, -1);
  sprintf(hex_string, "sp\t\t= %02X\n", memory_read(MEM_SFR_SP, MEM_TYPE_IRAM));
  gtk_text_buffer_insert_at_cursor(sfr_buffer, hex_string, -1);
  sprintf(hex_string, "dptr\t= %02X%02X\n", memory_read(MEM_SFR_DPH, MEM_TYPE_IRAM), memory_read(MEM_SFR_DPL, MEM_TYPE_IRAM));
  gtk_text_buffer_insert_at_cursor(sfr_buffer, hex_string, -1);
  sprintf(hex_string, "psw\t= %02X\n", memory_read(MEM_SFR_PSW, MEM_TYPE_IRAM));
  gtk_text_buffer_insert_at_cursor(sfr_buffer, hex_string, -1);
  sprintf(hex_string, "pc\t\t= %02X\n", mcu.pc);
  gtk_text_buffer_insert_at_cursor(sfr_buffer, hex_string, -1);
  sprintf(hex_string, "cycle\t= %02X\n", mcu.cycles);
  gtk_text_buffer_insert_at_cursor(sfr_buffer, hex_string, -1);
}

void code_show(GtkTextBuffer *rom_buffer) {
  word_t count = 0;
  for (int i = 0; i < MEM_CODE_SIZE; ++i) {
    char hex_string[10];
    if (count % 8 == 0) { // 每行显示8个数据
      gtk_text_buffer_insert_at_cursor(rom_buffer, "\n", -1);
      sprintf(hex_string, "Addr %02X:\t", count);
      gtk_text_buffer_insert_at_cursor(rom_buffer, hex_string, -1);
    }
    sprintf(hex_string, "%02X ", memory.code[i]); // 每个数据占两个字符位置，添加空格分隔
    gtk_text_buffer_insert_at_cursor(rom_buffer, hex_string, -1);
    count++;
  }
}

// 回调函数，点击按钮时执行 inst_exec_once() 函数并更新显示
void on_step_clicked(GtkWidget *widget, gpointer data) {
  inst_exec_once(&inst_encode);
  // 获取更新后的内存值，并更新显示
  GtkTextBuffer *iram_buffer = GTK_TEXT_BUFFER(data);
  GtkTextBuffer *sfr_buffer = global_sfr_buffer; // 假设有一个获取 sfr_buffer 的函数 get_sfr_buffer()

  GtkTextIter start, end;
  gtk_text_buffer_get_bounds(iram_buffer, &start, &end);
  gtk_text_buffer_delete(iram_buffer, &start, &end);
  iram_show(iram_buffer);

  gtk_text_buffer_get_bounds(sfr_buffer, &start, &end);
  gtk_text_buffer_delete(sfr_buffer, &start, &end);
  sfr_show(sfr_buffer);
}

void on_compile_clicked(GtkWidget *widget, gpointer data) {
  printf("begin compile\n");
  // 调用 inst_exec_once() 函数，更新内存值
  int count = 0;
  // Open the file for writing
  FILE *file = fopen("assembly.txt", "w");
  if (file == NULL) {
    perror("Error opening file");
  }
  mcu_reset(&mcu);
  while(count != 20000) {
    fprintf(file, "%02X: ", mcu.pc);
    inst_exec_once(&inst_encode);
    // printf("assembly = %s\n", assembly);
    fprintf(file, "%s\n", assembly);
    count ++;
  }
  
  fclose(file);
  // 打开文件以读取内容
  file = fopen("assembly.txt", "r");
  if (file == NULL) {
    perror("Error opening file");
    return;
  }

  // 读取文件内容并输出到 assembly code 窗口
  GtkTextBuffer *code_buffer = GTK_TEXT_BUFFER(data);
  char line[256];
  gtk_text_buffer_set_text(code_buffer, "\n", -1); // 清空原有内容
  while (fgets(line, sizeof(line), file)) {
    gtk_text_buffer_insert_at_cursor(code_buffer, line, -1);
  }

  fclose(file);
  printf("end compile\n");
  mcu_reset(&mcu);
}

void on_reset_clicked(GtkWidget *widget, gpointer data) {
  mcu_reset(&mcu);
  // 获取更新后的内存值，并更新显示
  GtkTextBuffer *iram_buffer = GTK_TEXT_BUFFER(data);
  GtkTextBuffer *sfr_buffer = global_sfr_buffer; // 假设有一个获取 sfr_buffer 的函数 get_sfr_buffer()

  GtkTextIter start, end;
  gtk_text_buffer_get_bounds(iram_buffer, &start, &end);
  gtk_text_buffer_delete(iram_buffer, &start, &end);
  iram_show(iram_buffer);

  gtk_text_buffer_get_bounds(sfr_buffer, &start, &end);
  gtk_text_buffer_delete(sfr_buffer, &start, &end);
  sfr_show(sfr_buffer);
}

void on_load_clicked(GtkWidget *widget, gpointer data) {
  GtkWidget *dialog;
  dialog = gtk_file_chooser_dialog_new("Open File", GTK_WINDOW(data), GTK_FILE_CHOOSER_ACTION_OPEN, "Open", GTK_RESPONSE_ACCEPT, "Cancel", GTK_RESPONSE_CANCEL, NULL);
  gint res = gtk_dialog_run(GTK_DIALOG(dialog));
  if (res == GTK_RESPONSE_ACCEPT) {
    char *filename;
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    memory_init(&memory);
    memory.code = hexfile_load(filename);
    // 获取更新后的内存值，并更新显示
    GtkTextBuffer *rom_buffer = GTK_TEXT_BUFFER(data);

    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(rom_buffer, &start, &end);
    gtk_text_buffer_delete(rom_buffer, &start, &end);
    code_show(rom_buffer);
    g_free(filename);
  }
  gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return EXIT_FAILURE;
  }
  char* filename = argv[1];
   printf("begin test: %s\n", filename);
  // 初始化GTK+
  gtk_init(&argc, &argv);
  mcu_init(&mcu);
  // 创建主窗口
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Memory and Assembly Viewer");
  gtk_window_set_default_size(GTK_WINDOW(window), 1400, 700);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  // 创建一个水平布局容器
  GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_container_add(GTK_CONTAINER(window), hbox);

  // 创建左侧笔记本控件
  GtkWidget *left_notebook = gtk_notebook_new();
  gtk_box_pack_start(GTK_BOX(hbox), left_notebook, TRUE, TRUE, 0);

  // 创建左侧标签页
  GtkWidget *iram_label = gtk_label_new("memory.iram");
  GtkWidget *iram_scrolled_window = gtk_scrolled_window_new(NULL, NULL); // 创建滚动窗口
  GtkWidget *iram_view = gtk_text_view_new();
  GtkTextBuffer *iram_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(iram_view));
  gtk_text_view_set_editable(GTK_TEXT_VIEW(iram_view), FALSE);
  iram_show(iram_buffer);
  gtk_container_add(GTK_CONTAINER(iram_scrolled_window), iram_view); // 将文本视图放入滚动窗口中
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(iram_scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS); // 设置滚动窗口的滚动策略
  gtk_notebook_append_page(GTK_NOTEBOOK(left_notebook), iram_scrolled_window, iram_label); // 将滚动窗口添加到笔记本控件中



  // 创建中左笔记本控件
  GtkWidget *middle_left_notebook = gtk_notebook_new();
  gtk_box_pack_start(GTK_BOX(hbox), middle_left_notebook, TRUE, TRUE, 0);

  // 创建中左标签页
  GtkWidget *code_label = gtk_label_new("assembly code");
  GtkWidget *code_scrolled_window = gtk_scrolled_window_new(NULL, NULL); // 创建滚动窗口
  GtkWidget *code_view = gtk_text_view_new();
  GtkTextBuffer *code_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(code_view));
  gtk_text_view_set_editable(GTK_TEXT_VIEW(code_view), FALSE);

  gtk_container_add(GTK_CONTAINER(code_scrolled_window), code_view); // 将文本视图放入滚动窗口中
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(code_scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS); // 设置滚动窗口的滚动策略
  gtk_notebook_append_page(GTK_NOTEBOOK(middle_left_notebook), code_scrolled_window, code_label); // 将滚动窗口添加到笔记本控件中

  // 创建中右笔记本控件
  GtkWidget *middle_right_notebook = gtk_notebook_new();
  gtk_box_pack_start(GTK_BOX(hbox), middle_right_notebook, TRUE, TRUE, 0);

  // 创建中右标签页
  GtkWidget *sfr_label = gtk_label_new("memory.sfr");
  GtkWidget *sfr_scrolled_window = gtk_scrolled_window_new(NULL, NULL); // 创建滚动窗口
  GtkWidget *sfr_view = gtk_text_view_new();
  GtkTextBuffer *sfr_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(sfr_view));
  gtk_text_view_set_editable(GTK_TEXT_VIEW(sfr_view), FALSE);
  sfr_show(sfr_buffer);
  gtk_container_add(GTK_CONTAINER(sfr_scrolled_window), sfr_view); // 将文本视图放入滚动窗口中
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sfr_scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS); // 设置滚动窗口的滚动策略
  gtk_notebook_append_page(GTK_NOTEBOOK(middle_right_notebook), sfr_scrolled_window, sfr_label); // 将滚动窗口添加到笔记本控件中
  global_sfr_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(sfr_view));
  // 创建右侧笔记本控件
  GtkWidget *right_notebook = gtk_notebook_new();
  gtk_box_pack_start(GTK_BOX(hbox), right_notebook, TRUE, TRUE, 0);

  // 创建右侧标签页
  GtkWidget *rom_label = gtk_label_new("memory.code");
  GtkWidget *rom_scrolled_window = gtk_scrolled_window_new(NULL, NULL); // 创建滚动窗口
  GtkWidget *rom_view = gtk_text_view_new();
  GtkTextBuffer *rom_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(rom_view));
  word_t *code;
  code = hexfile_load(filename);
  if (code == NULL) {
    printf("load hex file failed: %s", filename);
    exit(-1);
  }
  memory.code = code;
  code_show(rom_buffer);
  gtk_container_add(GTK_CONTAINER(rom_scrolled_window), rom_view); // 将文本视图放入滚动窗口中
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(rom_scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS); // 设置滚动窗口的滚动策略
  gtk_notebook_append_page(GTK_NOTEBOOK(right_notebook), rom_scrolled_window, rom_label); // 将滚动窗口添加到笔记本控件中


  // 创建一个垂直布局容器，用于放置按钮
  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);

  // 创建单步调试按钮
  GtkWidget *step_button = gtk_button_new_with_label("Step");
  g_signal_connect(step_button, "clicked", G_CALLBACK(on_step_clicked), iram_buffer);
  gtk_box_pack_start(GTK_BOX(vbox), step_button, FALSE, FALSE, 0);

  // 创建编译按钮
  GtkWidget *compile_button = gtk_button_new_with_label("Compile");
  g_signal_connect(compile_button, "clicked", G_CALLBACK(on_compile_clicked), code_buffer);
  gtk_box_pack_start(GTK_BOX(vbox), compile_button, FALSE, FALSE, 0);
  
  // 创建重置按钮
  GtkWidget *reset_button = gtk_button_new_with_label("Reset");
  g_signal_connect(reset_button, "clicked", G_CALLBACK(on_reset_clicked), iram_buffer);
  gtk_box_pack_start(GTK_BOX(vbox), reset_button, FALSE, FALSE, 0);

  // 创建加载按钮
  GtkWidget *load_button = gtk_button_new_with_label("Load");
  g_signal_connect(load_button, "clicked", G_CALLBACK(on_load_clicked), rom_buffer);
  gtk_box_pack_start(GTK_BOX(vbox), load_button, FALSE, FALSE, 0);

 
  
  

  // 显示窗口并开始主循环
  gtk_widget_show_all(window);
  gtk_main();

} 