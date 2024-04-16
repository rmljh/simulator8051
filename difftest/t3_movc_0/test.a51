	org 0x0
start:
	; MOVC A,@A+DPTR
	MOV DPTR, #table
	MOV A, #0x1
	MOVC A, @A+DPTR 
	MOV 0x20, A

	; MOVC A,@A+PC  
	MOV A, #0x2
	MOVC A, @A+PC
	MOV 0x21, A

	sjmp $
table:
	DB 0x12, 0x34
	
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
	dW  REG_A,		0x80
	dW  REG_B,		0x0
	;dW  REG_PSW,	0x1
	dW  REG_PC,		0xD
	dw  REG_DPTR,	0xf
	dw 	CYCLE,		12
		
	dW  0x20,		0x34
	dW  0x21,		0x80

	dw  REG_END,	0
	end		