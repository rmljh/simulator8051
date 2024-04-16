	org 0x0
start:
	; CLR A; CPL A
	CLR A
	MOV R0, A
	CPL A
	MOV R1, A
	
	; RL A, RLC A
	MOV A, #0x73
	RL A
	MOV R2, A
	
	MOV PSW, #0x80
	RLC A
	MOV R3, A

	MOV PSW, #0x00
	RLC A
	MOV R4, A

	; RR A, RRC A
	MOV A, #0x73
	RR A
	MOV R5, A
	
	MOV PSW, #0x80
	RRC A
	MOV R6, A
	
	MOV PSW, #0x00
	RRC A
	MOV R7, A

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
	dw  REG_R0,		0x00
	dw  REG_R1,		0xff
	dw  REG_R2,		0xe6
	dw  REG_R3,		0xcd
	dw  REG_R4,		0x9a
	dw  REG_R5,		0xb9
	dw  REG_R6,		0xdc
	dw  REG_R7,		0x6e

	dW  REG_SP,		0x7
	dW  REG_A,		0x6e
	dW  REG_B,		0x0
	dW  REG_PC,		0x20
	dw  REG_DPTR,	0x00
	dw 	CYCLE,		28
	dw  REG_PSW,			1

	dw  REG_END,	0
	end		