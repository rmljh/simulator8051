	org 0x0
start:
	MOV R0, #0x53
	MOV 0x20, #0x35
	
	; XRL A, Rn
	MOV A, #0xE6
	XRL A, R0
	MOV R1, A  ; R1 = 0xE6 & 0x53
	
	; XRL A, direct
	MOV A, #0xE7
	XRL A, 0x20
	MOV R2, A	; R2 = 0xE6 & 0x35
	
	; XRL A, @Ri
	MOV A, #0xA4
	MOV R0, #0x20
	XRL A, @R0
	MOV R3, A	; R3 = 0xA4 & 0x35
	
	; XRL A, #data
	MOV A, #0x73
	XRL A, #0x64
	MOV R4, A
	
	; XRL direct, A
	MOV A, #0x3C
	XRL 0x20, A
	MOV R5, 0x20
	
	; XRL direct, #data
	XRL 0x20, #0xF6
	MOV R6, 0x20
		
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
	dw  REG_R0,		0x20
	dw  REG_R1,		0xb5
	dw  REG_R2,		0xd2
	dw  REG_R3,		0x91
	dw  REG_R4,		0x17
	dw  REG_R5,		0x09
	dw  REG_R6,		0xff
	dw  REG_R7,		0x00

	dW  REG_SP,		0x7
	dW  REG_A,		0x3c
	dW  REG_B,		0x0
	dW  REG_PSW,	0x0
	dW  REG_PC,		0x24
	dw  REG_DPTR,	0x00
	dw 	CYCLE,		26

	dw  0x20, 0xff

	dw  REG_END,	0
	end		