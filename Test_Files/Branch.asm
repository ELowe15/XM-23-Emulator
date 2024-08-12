;
; Example X-Makina program
; Testing sum functions
; Called by mainline
;
; Evan Lowe
;
; July 12 2023
;

org	#800
;
Array	word	#0
	word	#FFAA
	word	#0
	word	$2
	word 	$1
	word 	#100
	word	#90
	word	#8888
	word	#1000

;
; Code space
;

	org	#900
branchBackward
	bl	branchForward	; Branch with negative offset

	org	#FF0
branchForward
	bra	branchAForward	;Branch always with postive offset

	org	#1000
Main
	movl	Array,R0
	movh	Array,R0
	ld	R0+,R1		; Copy [R0] to R1
	bl	branchBackward	; Branch with negaitve offset

branchAForward
	ld	R0+,R2		; Copy [R0] to R2
	cmp	R2,R1		;Compare R2 and R1 and set PSW bits
	beq	equal		;Branch if cmp resulted in 0 setting PSW.z = 1
	bra	branchAForward	;Branch always with negative offset

equal
	bne	notEqual	;Branch if PSW.z = 0
	ld	R0+,R1		; Copy [R0] to R1
	ld	R0+,R2		; Copy [R0] to R2
	cmp	R2,R1
	bra	equal

notEqual
	bc	carry		;Branch if PSW.c = 1
	ld	R0+,R2		; Copy [R0] to R2
	cmp	R2,R1
	bra	notEqual

carry
	bnc	noCarry		;Branch if PSW.c = 0
	add	R2,R1
	bra	carry

noCarry	
	bn	negative	;Branch if PSW.n = 1
	sub	R2,R1
	bra	noCarry

negative
	bge	ge		;Branch if pSW.n = 0 and PSW.v = 0
	ld	R0+,R1
	ld	R0+,R2
	cmp	R2,R1
	bra	negative

ge
	blt	done		;Branch if pSW.n = 1 and PSW.v = 1
	ld	R0+,R1
	ld	R0+,R2
	cmp	R1,R2
	bra	ge

done
	add	R1,R2




	end	Main		; End of program - first executable address is "Main"