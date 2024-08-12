;
; Sample X-Makina XM23 program
; Run through str and st instructions
; ECED 3403
; July 6 2023
;
	org	 #800
DATA	WORD	#2220
DATA2	WORD	#1234
ADDR	WORD	#2002
ADDR2	WORD	#3002
ADDR3	WORD	#4002
; Start of code area
;
	org	#1000
;
Start	movl	DATA,R0 ;Mov address of DATA into R0
	movh	DATA,R0 ;
	ld	R0,R1	;Load data from memory location in R0 into R1
	movl	ADDR,R0 ;Mov address of ADDR into R0
	movh	ADDR,R0
	ld	R0,R2	;Load data from memory location in R0 into R2
	movl	DATA2,R0 ;Mov address of DATA2 into R0
	movh	DATA2,R0
	ld	R0,R3	;Load data from memory location in R0 into R3
	str	R1,R2,#0 ;Store data in R1 into memory location from R2 with varrying offsets
	str	R1,R2,#2
	str	R1,R2,$-2
	str.b	R3,R2,#0
	str.b	R3,R2,#2
	str.b	R3,R2,$-2
	movl	ADDR2,R0 ;Mov address of ADDR2 into R0
	movh	ADDR2,R0
	ld	R0,R2	;Load data from memory location in R0 into R2
	st	R1,R2 ;Store data in R1 into memory location from R2 with varrying pre increments
	st	R1,+R2
	st	R1,-R2
	st.b	R3,R2
	st.b	R3,+R2
	st.b	R3,-R2
	movl	ADDR3,R0 ;Mov address of ADDR3 into R0
	movh	ADDR3,R0
	ld	R0,R2	;Load data from memory location in R0 into R2
	st	R1,R2+ ;Store data in R1 into memory location from R2 with varrying post increments
	st	R1,R2-
	st.b	R3,R2
	st.b	R3,R2+
	st.b	R3,R2-

;			
; End of program
;
	end	Start		; End of program - first executable address is "Start"