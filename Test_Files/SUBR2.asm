;
; Test subr call
;
; ECED 3403
;  1 Jun 23
;
LR	equ	R5
PC	equ	R7
;
	org	#1000
Test
	movls	#FF,R0
;
; Call Subr
;
	bl	Subr
; 
; End with ^C
;
Done	bra	Done
;
; Subroutine
;
	org	#2000
Subr
;
; Add 1 to R0
; R0 should be 0000
; C, Z set
;
	add	#1,R0
;
	mov	LR,PC
;
	end	Test
