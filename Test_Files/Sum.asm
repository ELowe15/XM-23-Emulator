;
; Example X-Makina program
; Testing sum functions
; Called by mainline
;
; L. Hughes and Evan Lowe
;
; July 12 2023 - 
;

org	#800
;
Array	word	#2
	word	#22F8
	word	#7F7F
	word	$-24
	word	$-33
	word	$-1
;
; Code space
;
	org	#1000
Main

	movl	Array,R0
	movh	Array,R0
	ld	R0+,R1	; Copy [R0] to R1
	add	$0,R2	; Add with a constant causing a zero, psw.z = 1
	add	$8,R2	; Add with a constant
	add	$-1,R2	; Add with a negative constant causing carry but no overflow
	add	R1,R2	; Add with register
	ld	R0+,R1	; Copy [R0] to R1
	add.b	R1,R2	; byte add causing carry
	ld	R0,R1	; Copy [R0] to R1
	add.b	R1,R2	; byte add to cause overflow and negative
	add	R1,R2	
	add	$1,R2	; word add to cause overflow

	end	Main		; End of program - first executable address is "Main"
