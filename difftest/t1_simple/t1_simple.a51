	org 0x0
start:
	nop
	nop
	sjmp next
	nop
next:	
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
	dW  REG_A,		0x0
	dW  REG_B,		0x0
	dW  REG_PC,		0x5
	dw  REG_DPTR,	0x0
	dw 	CYCLE,		6
	dW  REG_R0,		0x0
	dW  REG_R1,		0x0
	dW  REG_R2,		0x0
	dW  REG_R3,		0x0
	dW  REG_R4,		0x0
	dW  REG_R5,		0x0
	dW  REG_R6,		0x0
	dW  REG_R7,		0x0
	dw	REG_END
	end