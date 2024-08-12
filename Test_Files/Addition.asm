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
Array	word	#FFFF
	word	#D
	word	#E
	word	#C

;
; Code space
;
	org	#1000
Main

	movl	Array,R0
	movh	Array,R0
	ld	R0,R1	; Copy [R0] to R1
	addc	$16,R2	; Add with a constant causing a zero, psw.z = 1
	addc	R1,R2	; Add with register
	addc	$1,R2	; Add with a negative constant causing carry but no overflow
	sub	$4,R2
	sub	$-1,R2
	subc	$1,R2
	subc	$-1,R2
	ld	+R0,R1	; increment R0 and then Copy [R0] to R1
	cmp	R1,R2
	ld	+R0,R1	; increment R0 and then Copy [R0] to R1
	cmp	R1,R2
	ld	+R0,R1	; increment R0 and then Copy [R0] to R1
	cmp	R1,R2
	
	end	Main		; End of program - first executable address is "Main"