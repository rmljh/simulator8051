CC      = gcc
C_SRC   = $(wildcard *.c)
BIN     = $(patsubst %.c, %.o, $(C_SRC))
 
INCDIRS :=
LIBDIRS :=
 
INCDIRS += /home/orange/SDL2/SDL2-2.30.1/_install/include/SDL2
LIBDIRS += /home/orange/SDL2/SDL2-2.30.1/_install/lib
 
CFLAGS  = -g -Wall -I$(INCDIRS) -L$(LIBDIRS) -lSDL2 -lpthread -lm -ldl

main: seg7.o led.o main.o
	gcc  main.o led.o seg7.o ../built/inst.o ../built/memory.o ../built/hexfile_load.o ../built/mcu.o  -o main `sdl2-config --cflags --libs`

seg7: seg7.o
	gcc  seg7.o ../built/inst.o ../built/memory.o ../built/hexfile_load.o ../built/mcu.o  -o seg7 `sdl2-config --cflags --libs`

led: led.o
	gcc  led.o ../built/inst.o ../built/memory.o ../built/hexfile_load.o ../built/mcu.o  -o led `sdl2-config --cflags --libs`

main.o: main.c
	gcc -c -o main.o main.c `sdl2-config --cflags`

seg7.o: seg7.c
	gcc -c -o seg7.o seg7.c `sdl2-config --cflags`

led.o: led.c
	gcc -c -o led.o  led.c  `sdl2-config --cflags`

clean:
	$(RM) a.out $(BIN) led
 
.PHONY: all clean led
