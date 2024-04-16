	org 0x0
start:
	; acall/lcall/ret
	acall call_0
	lcall call_1
	sjmp next
call_0:
	mov 0x20, #0x30
	ret
	
	; sjmp/ajmp
next:
	mov 0x22, #0x32
	ajmp next1
	sjmp $
		
	org 0x200
call_1:
	mov 0x21, #0x31
	ret
next1:
	ljmp next2      ; ljmp
	sjmp $
next2:
	mov 0x23, #0x33

	; jmp @a+dptr
	mov a, #2
	mov dptr, #jmp_table
	jmp @a+dptr
jmp_table:
	ajmp func0
	ajmp func1
	ajmp func2

func0:
	sjmp $
func1:
	sjmp $
func2:
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
	dw  0x20,		0x30
	dw  0x21,		0x31
	dw  0x22,		0x32
	dw  0x23,		0x33

	dW  REG_SP,		0x7
	dW  REG_A,		0x2
	dW  REG_B,		0x00
	dW  REG_PSW,	0x1
	dW  REG_PC,		0x21A
	dw  REG_DPTR,	0x212
	dw 	CYCLE,		31

	dw  REG_END,	0
	end		