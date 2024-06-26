	org 0x0
start:
	; MOV direct,#data
	MOV DPTR, #0x1234
	
	; prepare
	MOV R0, #0x20    ; R0 = 0x20
	MOV 0x20, #0x11  ; (0x20) = 0x11
	MOV R1, #0x30    ; R1 = 0x30
	
	; MOV A,@Ri， MOV direct,@Ri
	MOV A, @R0		; a = 0x11
	MOV 0x21, @R0   ; (0x21) = A = 0x11
	
	; MOV @Ri,A
	MOV @R1, A		; (0x30) = 0x11

	; MOV @Ri,direct
	MOV R1, #0x31
	MOV @R1, 0x20	; (0x31) = (0x20)

	; MOV @Ri,#data
	MOV R1, #0x32
	MOV @R1, #0x78	; (0x32) = 0x78
	
	; MOV C,bit, MOV bit,C
	MOV PSW, #0x80		; C = 0x1
	MOV 0x0, C
	MOV R0, PSW			; R0 = 0x80
	MOV PSW, #0x00		; C = 0x1
	MOV C, 0x0
	MOV R1, PSW			; R1 = 0x00
	
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
	dW  0x20,		0x11
	dW  0x21,		0x11

	dW  0x30,		0x11
	dW  0x31,		0x11
	dW  0x32,		0x78

	dW  REG_SP,		0x7
	dW  REG_A,		0x11
	dW  REG_B,		0x0
	dW  REG_PC,		0x24
	dw  REG_DPTR,	0x1234
	dw 	CYCLE,		28
	dW  REG_R0,		0x80
	dW  REG_R1,		0x80
		
	dw  REG_END,	0
	end		