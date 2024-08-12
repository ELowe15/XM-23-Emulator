;
; Sample X-Makina XM23 program
; Run through movl, movh, movls, molvz, ldr and ld instructions
; ECED 3403
; July 6 2023
;
	org	 #800
	WORD	#1110
DATA	WORD	#2220
	WORD	#3330
ADDR	WORD	#2000
; Start of code area
;
	org	#1000
;
Start	movl	DATA,R0 ;Mov address of DATA into R0
	movh	DATA,R0 ;
	movls	DATA,R1 ;Test movls and movlz on R1
	movlz	DATA,R1 ;
	ldr	R0,#0,R1 ;Load content at memory location in R0 into R1 with varrying relative addresses
	ldr	R0,#2,R1 ;
	ldr	R0,$-2,R1 ;
	ldr.b	R0,#0,R1 ;Load relative with bytes
	ldr.b	R0,#2,R1
	ldr.b	R0,$-2,R1
	movlz	#0,R1 ;Set R1 to 0
	ld	R0,R1 ;Load content at memory location in R0 into R1 with varrying pre increments
	ld	+R0,R1
	ld	-R0,R1
	ld.b	R0,R2
	ld.b	+R0,R2
	ld.b	-R0,R2
	movlz	#0,R1
	movlz	#0,R2
	ld	R0+,R1 ;Load content at memory location in R0 into R1 with varrying post increments
	ld	R0-,R1
	ld.b	R0,R2
	ld.b	R0+,R2
	ld.b	R0-,R2
;			
; End of program
;
	end	Start		; End of program - first executable address is "Start"