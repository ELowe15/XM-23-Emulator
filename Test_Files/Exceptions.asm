;
; Exception tests
; ECED 3403
; 18 Jul 23
;
LR	equ	R5	; Link register
SP	equ	R6	; Stack pointer
PC	equ	R7	; Program counter
;
NUL	equ	#0	; NUL char
SCR_SET	equ	#10	; SCR initialization: ENA=1 I/O=0 IE=0
SCR_DBA	equ	#4	; SCR - data buffer available status bit (output)
;
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
	org	#0
ClkCSR	bss	$1
ClkDR	bss	$1
KbCSR	bss	$1
KbDR	bss	$1
ScrCSR	bss	$1
ScrDR	bss	$1
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
;
	setpri	#3
;
; ************************************************************
; Illegal instruction
;
;	word	#4fff	; HCF	
;
; ************************************************************
; Illegal address (PC)
;
;	movlz	#01,PC
;
; ************************************************************
; Illegal priority (raise from 3 to 6)
;
	setpri	#6
;
; ************************************************************
; SVC 
;
	svc	#0	; Trap through vector 0
;
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
; This should fail, #1 is priority 4
	svc	#1	; Trap through vector 1 
	mov	LR,PC	; return
;
SVC1
; Priority 4
	movls	#AA,R0
	mov	LR,PC
;
; ILL_INST (8) - illegal instruction
; 
Ill_Inst
	movlz	#0,R0
	bra	Flt_Disp
;
; ILL_ADDR (9) - illegal address (PC on odd boundary)
;
Ill_Addr
	movlz	#1,R0
	bra	Flt_Disp
;
; PRI_FLT (10) - Priority fault - priority X to a priority < X
;
PriFault
	movlz	#2,R0
	bra	Flt_Disp
;
; DBL_FLT (11) - Double fault
;
DblFault
	movlz	#3,R0
	bra	Flt_Disp
;
; Display msg FltMsgTbl[R0]
; R0 contains offset into FltMsgTbl
; Polled output
;
Flt_Disp
	add	R0,R0		; R0 = R0 * 2
	movls	FltMsgTbl,R2	; R2 = addr FltMsgTbl
	add	R0,R2		; R2 = R1 + R0 (R2 is table addr)
	ld	R2,R2		; R2 = addr of fault msg
;
	movlz	ScrCSR,R0	; R0 = addr ScrCSR
	movl	SCR_SET,R1	; R1 = SCR_SET
	st.b	R1,R0		; [ScrCSR] = SCR_SET
Flt_Disp0
	ld.b	R2+,R3		; R3 = [R2]; R2++
	cmp.b	NUL,R3		; R3 = NUL ?
	beq	Flt_Disp2	; Yes: Goto Flt_Disp2 
Flt_Disp1
	ld.b	R0,R1		; R1 = [ScrCSR]
	and.b	SCR_DBA,R1	; R1.DBA = 1?
	beq	Flt_Disp1
; Output
	str.b	R3,R0,$1	; [ScrDR] = R2
	bra	Flt_Disp0
;
Flt_Disp2
	setcc	s		; put CPU to sleep
	bra	Flt_Disp2
;
FltMsgTbl
	word	Ill_msg		; 0
	word	Ill_adr		; 1
	word	Pri_flt		; 2
	word	Dbl_flt		; 3
;
Ill_msg	ascii	"Fault 8\0"
Ill_adr	ascii	"Fault 9\0"
	byte	NUL
Pri_flt	ascii	"Fault 10"
	byte	NUL
Dbl_flt	ascii	"Fault 11"
	byte	NUL
;
; Exception addresses
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
