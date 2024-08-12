;
; Sample X-Makina XM23 program
; Run through all 41 instructions
; ECED 3403
; July 4 2023
;
; Start of code area
;
	org	#1000
;
Start	bl	#12
	bl	#1200	
	beq	#FF8	
	bne	#1002		
	bc	#1004
	bnc	#1000
	bn 	#1200
	bge	#1002
	blt	#1000
	bra	#1000
	add	R0,R1
	addc	$2,R2
	sub	R4,R3
	subc	$-1,R5
	dadd	R3,R6
	cmp.b	R0,R1
	xor.b	$8,R1
	and.b	$-1,R1
	or	R0,R1
	bit 	R0,R1
	bic	R0,R1
	bis	R0,R1
	mov	R0,R1	
	movl	$10,R0
	ldr	R0,#10,R1

			
; End of program
;
	end	Start		; End of program - first executable address is "Start"
