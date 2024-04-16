	org 0x0
start:
	;MOVX @Ri,A
	MOV R0, #0x20
	MOV A, #0x12
	MOVX @R0, A  ;(0x20) = 0x12

	; MOVX A,@Ri
	MOV A, #0
	MOVX A, @R0  ;A = 0x12
	MOV 0x20, A

	; MOVX @DPTR,A
	MOV DPTR, #0x30
	MOV A, #0x23
	MOVX @DPTR, A  ;(0x30) = 0x23

	; MOVX A,@DPTR
	MOV A, #0
	MOVX A, @DPTR  ;A = 0x23
	MOV 0x21, A
	sjmp $
table:
	DB  0x12
	DB  0x13

	
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
	dw  0x20, 0x12
	dw  0x21, 0x23

	dW  REG_SP,		0x7
	dW  REG_A,		0x23
	dW  REG_B,		0x0
	dW  REG_PC,		0x15
	dw  REG_DPTR,	0x30
	dw  CYCLE,		19
	
	dw  REG_END,	0
	end		