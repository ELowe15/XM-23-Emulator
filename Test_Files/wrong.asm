;
; Sample X-Makina XM23 program
; ArrayInit.xme without a st instruction
; ECED 3403
; July 22 2023

ST_LOC	equ	#818
; Start of data area
;
	org	#800
Array	word	#1234
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
	movl	ST_LOC,R1
	movh	ST_LOC,R1	
; 
Loop
	cmp	#1,R2		; R0 = R1? ('Z')
	beq	Done		; Equal - leave
	st	R0,R1
	ld.b	R1+,R2
	add	$1,R1
	bra	Loop		; Repeat loop
;
; End of program
;
Done	movlz	'*',R1
	bra	Done
;
	end	Start		; End of program - first executable address is "Start"
