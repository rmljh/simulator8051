INCDIRS := D:\SDL2-2.30.2\x86_64-w64-mingw32\include\SDL2
LIBDIRS := D:\SDL2-2.30.2\x86_64-w64-mingw32\lib

CFLAGS = -g -O0 -I$(INCDIRS)
LDFLAGS = -L$(LIBDIRS) -lmingw32 -lSDL2main -lSDL2 -lpthread -lm

diff_instr: diff_instr.o hexfile_load.o inst.o mcu.o memory.o
	gcc -g -O0 .\built\diff_instr.o .\built\hexfile_load.o .\built\inst.o .\built\mcu.o .\built\memory.o -o diff_instr.exe

diff_program: diff_program.o hexfile_load.o inst.o mcu.o memory.o
	gcc -g -O0 .\built\diff_program.o .\built\hexfile_load.o .\built\inst.o .\built\mcu.o .\built\memory.o -o diff_program.exe

led: led.o hexfile_load.o inst.o mcu.o memory.o
	gcc $(CFLAGS) $(LDFLAGS) .\built\led.o .\built\hexfile_load.o .\built\inst.o .\built\mcu.o .\built\memory.o -o led.exe

memory_test: memory.o mcu.o memory_test.o
	gcc -g -O0 .\built\memory.o .\built\mcu.o .\built\memory_test.o -o memory_test.exe

diff_instr.o: .\monitor\diff_instr.c
	gcc -g -c -O0 .\monitor\diff_instr.c -o .\built\diff_instr.o

diff_program.o: .\monitor\diff_program.c
	gcc -g -c -O0 .\monitor\diff_program.c -o .\built\diff_program.o

hexfile_load.o: .\tools\hexfile_load.c
	gcc -g -c -O0 .\tools\hexfile_load.c -o .\built\hexfile_load.o 

inst.o: .\isa\inst.c
	gcc -g -c -O0 .\isa\inst.c -o .\built\inst.o 

mcu.o: .\mcu\mcu.c
	gcc -g -c -O0 .\mcu\mcu.c -o .\built\mcu.o 

memory.o: .\memory\memory.c
	gcc -g -c -O0 .\memory\memory.c -o .\built\memory.o 

memory_test.o: .\monitor\memory_test.c
	gcc -g -c -O0 .\monitor\memory_test.c -o .\built\memory_test.o 

led.o: .\SDL\led.c
	gcc -g -c -O0 -I$(INCDIRS) .\SDL\led.c  -o .\built\led.o
