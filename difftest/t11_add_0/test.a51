	org 0x0
start:
	; no ac, cy, ov
	MOV A, #0x36
	ADD A, #0x12
	MOV 0x20, PSW
	MOV 0x21, A
	
	;cy
	MOV A, #0xC6
	ADD A, #0xC3
	MOV 0x22, PSW
	MOV 0x23, A

	;ac
	MOV A, #0x19
	ADD A, #0x2E
	MOV 0x24, PSW
	MOV 0x25, A

	;ov, bit6->bit7
	MOV A, #0x48
	ADD A, #0x72
	MOV 0x26, PSW
	MOV 0x27, A
		
	;ov+cy, bit7->bit8
	MOV A, #0xC8
	ADD A, #0xA2
	MOV 0x28, PSW
	MOV 0x29, A

	;ov, cy, ac
	MOV A, #0xC8
	ADD A, #0xA8
	MOV 0x2A, PSW
	MOV 0x2B, A
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
	dw  0x20,		0x00
	dw  0x21,		0x48
	dw  0x22,		0x81
	dw  0x23,		0x89
	dw  0x24,		0x40
	dw  0x25,		0x47
	dw  0x26,		0x05
	dw  0x27,		0xBA
	dw  0x28,		0x84
	dw  0x29,		0x6A
	dw  0x2A,		0xC5
	dw  0x2B,		0x70

	dW  REG_SP,		0x7
	dW  REG_A,		0x70
	dW  REG_B,		0x0
	dW  REG_PSW,	0xc5
	dW  REG_PC,		0x36
	dw  REG_DPTR,	0x00
	dw 	CYCLE,		32

	dw  REG_END,	0
	end		