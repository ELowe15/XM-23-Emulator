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
Array	word	#AAAA
	word	#F0F1
	word	#F157
	word	#FF14
	word	#E

;
; Code space
;
	org	#1000
Main

	movl	Array,R0
	movh	Array,R0
	ld	R0,R1	; Copy [R0] to R1
	ld	R0+,R2	; Copy [R0] to R1
	xor	R1,R2
	ld	R0+,R2	; Copy [R0] to R1
	xor	R1,R2
	xor	$-1,R2
	xor.b	R1,R2
	xor.b	$8,R2
	and	R1,R2
	ld	R0,R2	; Copy [R0] to R1
	and.b	R1,R2
	or	R1,R2
	ld	R0+,R2	; Copy [R0] to R1
	or.b	R1,R2
	ld	R0+,R2	; Copy [R0] to R1
	bit	$1,R2
	bit	$2,R2
	bic	$4,R2
	ld	R0,R1	; Copy [R0] to R1
	bic	R1,R2
	bis	$4,R2
	bis	R1,R2
	
	end	Main		; End of program - first executable address is "Main"