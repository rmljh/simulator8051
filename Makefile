INCDIRS :=
LIBDIRS :=
 
INCDIRS += /home/orange/SDL2/SDL2-2.30.1/_install/include/SDL2
LIBDIRS += /home/orange/SDL2/SDL2-2.30.1/_install/lib


main: seg7.o led.o main.o inst.o memory.o hexfile_load.o mcu.o
	gcc ./built/main.o ./built/led.o ./built/seg7.o ./built/inst.o ./built/memory.o ./built/hexfile_load.o ./built/mcu.o -o main `sdl2-config --cflags --libs`

diff_instr: diff_instr.o hexfile_load.o inst.o mcu.o memory.o
	gcc -g -O0 ./built/diff_instr.o ./built/hexfile_load.o ./built/inst.o ./built/mcu.o ./built/memory.o -o diff_instr

diff_program: diff_program.o hexfile_load.o inst.o mcu.o memory.o
	gcc -g -O0 ./built/diff_program.o ./built/hexfile_load.o ./built/inst.o ./built/mcu.o ./built/memory.o -o diff_program

led: led.o hexfile_load.o inst.o mcu.o memory.o
	gcc `sdl2-config --cflags --libs` ./built/led.o ./built/hexfile_load.o ./built/inst.o ./built/mcu.o ./built/memory.o -o led

monitor: monitor.o hexfile_load.o inst.o mcu.o memory.o
	gcc -g -O0 ./built/monitor.o ./built/hexfile_load.o ./built/inst.o ./built/mcu.o ./built/memory.o -o Gtkmonitor `pkg-config --libs --cflags gtk+-3.0`

memory_test: memory.o mcu.o memory_test.o
	gcc -g -O0 ./built/memory.o ./built/mcu.o ./built/memory_test.o -o memory_test

monitor.o: ./monitor/monitor.c
	gcc -g -c -O0 `pkg-config --libs --cflags gtk+-3.0` ./monitor/monitor.c -o ./built/monitor.o 

diff_instr.o: ./monitor/diff_instr.c
	gcc -g -c -O0 ./monitor/diff_instr.c -o ./built/diff_instr.o

diff_program.o: ./monitor/diff_program.c
	gcc -g -c -O0 ./monitor/diff_program.c -o ./built/diff_program.o

hexfile_load.o: ./tools/hexfile_load.c
	gcc -g -c -O0 ./tools/hexfile_load.c -o ./built/hexfile_load.o 

inst.o: ./isa/inst.c
	gcc -g -c -O0 ./isa/inst.c -o ./built/inst.o 

mcu.o: ./mcu/mcu.c
	gcc -g -c -O0 ./mcu/mcu.c -o ./built/mcu.o 

memory.o: ./memory/memory.c
	gcc -g -c -O0 ./memory/memory.c -o ./built/memory.o 

memory_test.o: ./monitor/memory_test.c
	gcc -g -c -O0 ./monitor/memory_test.c -o ./built/memory_test.o 

led.o: ./SDL/led.c
	gcc -g -c -O0 `sdl2-config --cflags` ./SDL/led.c  -o ./built/led.o

seg7.o: ./SDL/seg7.c
	gcc -g -c -O0 `sdl2-config --cflags` ./SDL/seg7.c  -o ./built/seg7.o

main.o: ./SDL/main.c
	gcc -g -c -O0 `sdl2-config --cflags` ./SDL/main.c  -o ./built/main.o
.PHONY: clean
clean:
	rm *.exe
