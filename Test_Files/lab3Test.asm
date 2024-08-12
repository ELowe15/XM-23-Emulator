;
; Sample X-Makina XM23 program
; Run through 6 instructions
; ECED 3403
; July 6 2023
;
	org	 #800
	WORD	#1110
DATA	WORD	#2220
	WORD	#3330
; Start of code area
;
	org	#1000
;
Start	movl	DATA,R0
	movh	DATA,R0
	movls	DATA,R1
	movlz	DATA,R1
	ldr	R0,#0,R1
	ldr	R0,#2,R1
	ldr	R0,$-2,R1
	ldr.b	R0,#0,R1
	ldr.b	R0,#2,R1
	ldr.b	R0,$-2,R1
	ld	R0,R2
	ld	+R0,R2
	ld	-R0,R2
	ld.b	R0,R3
	ld.b	+R0,R3
	ld.b	-R0,R3
	ld	R0,R3
	ld	R0+,R3
	ld	R0-,R3
	ld.b	R0,R4
	ld.b	R0+,R4
	ld.b	R0-,R4
	
	
	
			
; End of program
;
	end	Start		; End of program - first executable address is "Start"