diff: diff.o hexfile_load.o inst.o mcu.o memory.o
	gcc -g -O0 .\built\diff.o .\built\hexfile_load.o .\built\inst.o .\built\mcu.o .\built\memory.o -o diff.exe

diff.o: .\monitor\diff.c
	gcc -g -c -O0 .\monitor\diff.c -o .\built\diff.o

hexfile_load.o: .\tools\hexfile_load.c
	gcc -g -c -O0 .\tools\hexfile_load.c -o .\built\hexfile_load.o 

inst.o: .\isa\inst.c
	gcc -g -c -O0 .\isa\inst.c -o .\built\inst.o 

mcu.o: .\mcu\mcu.c
	gcc -g -c -O0 .\mcu\mcu.c -o .\built\mcu.o 

memory.o: .\memory\memory.c
	gcc -g -c -O0 .\memory\memory.c -o .\built\memory.o 
