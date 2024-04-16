	org 0x0
start:
	;JZ/JNZ
	MOV A, #0
	JNZ next0
	MOV 0x20, #0x30
	JZ next0
	MOV 0x20, #0x31
	sjmp $
next0:
	MOV A, #1
	JZ next1
	MOV 0x21, #0x30
	JNZ next1
	MOV 0x21, #0x31
	sjmp $
next1:
	
	; DJNZ  Rn,rel
	MOV R0, #8
	MOV A, #0
loop0:	
	ADD A, #1
	DJNZ R0, loop0
	MOV 0x22, A
	
	; DJNZ  direct,rel
	MOV 0x40, #8
	MOV A, #0x12
loop1:	
	ADD A, #1
	DJNZ 0x40, loop1
	MOV 0x23, A

	; CJNE A,direct,rel
	MOV A, #0x20
	MOV 0x41, #0x20
	CJNE A, 0x41, die
	CJNE A, #0x33, next5
	MOV 0x24, #0x45
	SJMP $
next5:
	MOV 0x24, #0x55

	MOV R0, #0x12
	CJNE R0, #0x34, cjne_next1
	SJMP $
cjne_next1:
	MOV 0x40, #0x80
	MOV R0, #0x40
	CJNE @R0, #0x45, die
	SJMP $
	
die: SJMP $

; for test
REG_SP		EQU	0x1000
REG_A		EQU	0x1001
REG_B		EQU	0x1002
REG_PSW		EQU	0x1003
REG_PC		EQU	0x1004
REG_DPTR	EQU	0x1005
CYCLE		EQU 0x1006
REG_R0		EQU	0x2000
REG_R1		EQU	0x2001
REG_R2		EQU	0x2002
REG_R3		EQU	0x2003
REG_R4		EQU	0x2004
REG_R5		EQU	0x2005
REG_R6		EQU	0x2006
REG_R7		EQU	0x2007
REG_END		EQU	0x2FFF
	org 0x600
	dW  REG_SP,		0x7
	dW  REG_A,		0x20
	dW  REG_B,		0x00
	dW  REG_PSW,	0x1
	dW  REG_PC,		0x56
	dw  REG_DPTR,	0x00
	dw 	CYCLE,		88

	dw  0x20,		0x30
	dw  0x21,		0x30
	dw  0x22,		0x08
	dw  0x23,		0x1A
	dw  0x24,		0x55

	dw  REG_END,	0
	end		