	org 0x0
start:
	MOV 0x20, #0
	MOV 0x21, #0
	MOV 0x22, #0
	
	SETB 0x0		; SETB BIT
	SETB C			; SETB C
	MOV 0x1, C		; MOV bit, C
	
	CLR C			; CLRC
	MOV 0x2, C		
	
	CPL C			; cpl c
	MOV 0x3, C
	
	MOV 0x4, C
	CLR 0x4			; CLR bit
	
	MOV 0x5, C
	CPL 0x5			; cpl bit	
	
	SETB 0x6	
	MOV C, 0x6		; mov c, bit
	MOV 0x7, C
	
	;logic ANL
	SETB 0x40
	SETB C
	ANL C, 0x40		; ANL C,bit
	MOV 0x8, C

	ANL C, /0x40	; ANL C,/bit
	MOV 0x9, C

	;logic ORL
	SETB 0x40
	SETB C
	ORL C, 0x40		; ORL C,bit
	MOV 0x10, C

	ORL C, /0x40	; ORL C,/bit
	MOV 0x11, C
	
	; JC/JNC
	SETB C
	JNC next0
	JC next0
	SJMP $
next0:
	CLR C
	JC next1
	JNC next1
	SJMP $
next1:

	; JB/JNB
	SETB 0x40
	JNB 0x40, next2
	JB 0x40, next2
	SJMP $
next2:
	CLR C
	JB 0x40, next3
	JNB 0x40, next3
	SJMP $
next3:

	; JBC
	CLR 0x40
	SETB 0x41
	JBC 0x40, next4
	JBC 0x41, next5
	SJMP $
next4:
	SJMP $
next5:
	MOV 0x12, C
	SJMP $

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
	dw  0x20,		0xCB
	dw  0x21,		0x01
	dw  0x22,		0x03

	dW  REG_SP,		0x7
	dW  REG_A,		0x00
	dW  REG_B,		0x00
	dW  REG_PSW,	0x00
	dW  REG_PC,		0x69
	dw  REG_DPTR,	0x00
	dw 	CYCLE,		74

	dw  REG_END,	0
	end		