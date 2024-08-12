;
; setpri test
; ECED 3403
; 28 Jul 23
;
LR	equ	R5	; Link register
SP	equ	R6	; Stack pointer
PC	equ	R7	; Program counter
;p
; System faults
;
ILL_INST	equ	#FFE0	; Illegal instruction vector (8)
ILL_ADDR	equ	#FFE4	; Illegal PC address vector (9)
PRI_FLT		equ	#FFE8	; Priority fault vector (10)
DBL_FLT		equ	#FFEC	; Double fault (11)
ILL_VECT12	equ	#FFF0
ILL_VECT13	equ	#FFF4
ILL_VECT14	equ	#FFF8
RESET_VECT	equ	#FFFC
;
	org	#600
; Reset entry point
Reset	mov	R0,R0	; Do something pointless
	bra	Reset
;
	org	#1000
Start
;
; Default priority is 7
;
	movl	PSW0,SP
	movh	PSW0,SP
;
	movls	$0,LR	; Show on stack
	setpri	#2
	svc	#1

Done
	bra 	Done
;
	org	#fe00
Stop
	bra	Stop
;
; ************************************************************
; ************************************************************
; Put handler code in high memory
;
	aorg	#ff00
SVC0
; Priority 5
	movls	#FF,R0	; do something
	mov	LR,PC	; return
;
SVC1
; Priority 4
	movls	#AA,R0
	svc	#0
	movls	#FA,R0
	mov	LR,PC
;
; ILL_INST (8) - illegal instruction
; 
Ill_Inst
	movlz	$8,R0
	bra	Stop
;
; ILL_ADDR (9) - illegal address (PC on odd boundary)
;
Ill_Addr
	movlz	$9,R0
	bra	Stop
;
; PRI_FLT (10) - Priority fault - priority X to a priority < X
;
PriFault
	movlz	$10,R0
	bra	Stop
;
; DBL_FLT (11) - Double fault
;
DblFault
	movlz	$11,R0
	bra	Stop
;
; Exception addresses
;
;
	org	#ffc0		; 0 - SVC vector #0
;
PSW0	word	#00A0		; Priority 5
PC0	word	SVC0
;
PSW1	word	#0080		; Priority 4
PC1	word	SVC1
;
	org	ILL_INST	; 8
PSW8	word	#00E0
PC8	word	Ill_Inst
;
	org	ILL_ADDR	; 9
PSW9	word	#00E0
PC9	word	Ill_Addr
;
	org	PRI_FLT		; 10
PSW10	word	#00E0		; Priority 7 - highest
PC10	word	PriFault
;
	org	DBL_FLT		; 10
PSW11	word	#00E0		; Priority 7 - highest
PC11	word	DblFault
;
; Reset vector
;
	org	RESET_VECT
RESET_PSW	word	#4060	; Prev: 2, Curr: 3
RESET_ADDR	word	Reset	; Reset address the program
;
	end	Start
