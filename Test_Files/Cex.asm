;
; Sample X-Makina XM23 program
; Test CEX
; ECED 3403
; July 24 2023

ST_LOC	equ	#818
; Start of data area
;
	org	#800
Array	word	#1
	word	#123
	word	#12
	word	#1
;
; Start of code area
;
	org	#1000
;
Start	movl	Array,R0
	movh	Array,R0	
	ld	R0+,R1
	cmp	#1,R1
	cex	EQ,$2,$3 ;Equal is true
	add	#1,R1
	add	#1,R1
	add	#1,R1
	add	#1,R1
	add	#1,R1
	cmp	#1,R1

	cex	EQ,$2,$3 ;Equal is false
	add	#1,R1
	add	#1,R1
	add	#1,R1
	add	#1,R1
	add	#1,R1

	cex	NE,$1,$1 ;Not equal is true, psw.z = 0
	add	$-1,R1
	add	$1,R1

	cex	CS,$1,$1 ;Carry set is true
	add	$1,R1
	sub	$1,R1

	cex	CC,$1,$1 ;Carry clear is true
	add	$1,R1
	sub	$1,R1

	cex	MI,$1,$1 ;Minus is false
	add	$1,R1
	sub	$1,R1

	cex	PL,$1,$1 ;PLus is true
	add	$1,R1
	sub	$1,R1

	cex	VS,$1,$1 ;Overflow is false
	add	$1,R1
	sub	$1,R1

	cex	VC,$1,$1 ;No overflow is true
	add	$-1,R1
	add	$1,R1

	cex	HI,$1,$1 ;Higher is true, PSW.c = 1 and PSW.z = 0
	add	$1,R1
	sub	$1,R1

	cex	LS,$1,$1 ;Lower or same is false
	add	$1,R1
	sub	$1,R1

	cex	GE,$1,$1 ;Greater than or equal is true, PSW.n = PSW.v
	add	$1,R1
	sub	$1,R1

	cex	LT,$1,$1 ;Less than is false, PSW.n = PSW.v
	add	$1,R1
	sub	$1,R1

	cex	GT,$1,$1 ;Greater than is true, PSW.z = 0 and PSW.n = PSW.v
	add	$1,R1
	sub	$1,R1

	cex	LE,$1,$1 ;Less than or equal is false, PSW.z = 0 and PSW.n = PSW.v
	add	$1,R1
	sub	$1,R1

	cex	TR,$1,$1 ;Always true
	add	$1,R1
	sub	$1,R1

	cex	FL,$1,$1 ;Always false
	add	$1,R1
	sub	$1,R1
	
	cex	NE,$1,$5
	bra	branch

	org 	#1200
branch	
	add	$1,R1
	add	$1,R1
	add	$1,R1
;
; End of program
;
Done	movlz	'*',R1
	bra	Done
;
	end	Start		; End of program - first executable address is "Start"
