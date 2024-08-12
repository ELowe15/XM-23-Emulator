;
; Sample X-Makina XM23 program
; Load values from one location and store in another
; ECED 3403
; July 22 2023

ST_LOC	equ	#910
; Start of data area
;
	org	#800
Array	word	#1234
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
	ld.b	R0+,R2
	st.b	R2,R1+
	bra	Loop
;
; End of program
;
Done	movlz	'*',R1
	bra	Done
;
	end	Start		; End of program - first executable address is "Start"
