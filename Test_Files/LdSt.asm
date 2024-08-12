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
	cex	EQ,2,3
	add	#1,R1
	add	#1,R1
	add	#1,R1
	add	#1,R1
	add	#1,R1
	cmp	#1,R1
	cex	EQ,2,3
	add	#1,R1
	add	#1,R1
	add	#1,R1
	add	#1,R1
	add	#1,R1
	ld	R0+,R1
;
; End of program
;
Done	movlz	'*',R1
	bra	Done
;
	end	Start		; End of program - first executable address is "Start"
