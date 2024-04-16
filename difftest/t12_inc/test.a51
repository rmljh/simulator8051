	org 0x0
start:
	; inc a
	MOV A, #0x78
	INC A
	MOV 0x20, A
	
	; INC Rn
	MOV R5, #0x32
	INC R5
	MOV 0x21, R5
	
	; INC direct
	MOV 0x22, 0x21
	INC 0x22
	
	; inc @ri
	MOV R1, #0x23
	MOV 0x23, #0x43
	INC @R1
	
	; inc dptr
	MOV DPTR, #0x1234
	INC DPTR
	
	sjmp $

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
	dW  REG_A,		0x79
	dW  REG_B,		0x0
	dW  REG_PSW,	0x01
	dW  REG_PC,		0x19
	dw  REG_DPTR,	0x1235
	dw 	CYCLE,		20

	dw  0x20,		0x79
	dw  0x21,		0x33
	dw  0x22,		0x34
	dw  0x23,		0x44
	dw  REG_END,	0
	end		