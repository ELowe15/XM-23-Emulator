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
	word	#F323

;
; Code space
;
	org	#1000
Main
	movl	Array,R0
	movh	Array,R0
	ld	R0+,R1	; Copy [R0] to R1
	ld	R0+,R2	; Copy [R0] to R1
	mov	R1,R3
	mov	R2,R3
	mov.b	R1,R3
	swap	R2,R3
	sra	R3
	sra.b	R3
	rrc	R3
	add	$-1,R3
	rrc.b	R3
	comp	R3
	comp.b	R3
	swpb	R3
	sxt	R3	;SXT a 1
	ld	R0,R3
	sxt	R3	;SXT a 0
	
	
	end	Main		; End of program - first executable address is "Main"