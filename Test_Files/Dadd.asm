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
Array	word	#8089
	word	#4321
	word	#9934
	word	#0
	word	#8000
;
; Code space
;
	org	#1000
Main

	movl	Array,R0
	movh	Array,R0
	ld	R0+,R1	; Copy [R0] to R1
	ld	R0,R2	; Copy [R0] to R1
	dadd	R1,R2
	ld	R0+,R2	; Copy [R0] to R1
	dadd	R1,R2
	ld	R0-,R2	; Copy [R0] to R1
	dadd.b	R1,R2
	ld	R0+,R2	; Copy [R0] to R1
	dadd.b	R1,R2
	dadd	#10,R2
	ld	+R0,R1
	ld	R0+,R2
	dadd	R1,R2
	ld	R0,R1
	dadd	R1,R2
	
	end	Main		; End of program - first executable address is "Main"