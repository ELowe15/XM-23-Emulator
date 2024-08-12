;
; Bit count #2
; Count number of bits in word by repeated shifts
; Use carry to indicate state of LSBit
; ECED 3403
; 11 May 2023
;
Count	equ	R0	; Count used in place of R0
DataVal	equ	R1	; DataVal - R1
DataAdr	equ	R1	; DataAdr - R1
;
	org	#1000
Start
;
; Get word with bits to count
; Get address into DataAdr
;
	movl	Data,DataAdr
	movh	Data,DataAdr	; R1 <- addr Data
;
; Get value into DataVal
;
	ld	DataAdr,DataVal	; R1 <- mem[R1]
;
; Count = 0
;
	movlz	#0,Count
;
; Check bits until value is zero
;
BC_Loop	cmp	#0,DataVal
	beq	Done
;
; Execute if not zero:
; Clear Carry bit
;
	clrcc	c
;
; Access LSBit
;
	rrc	DataVal		; C = R1.b0; R1 = R1 >> 1
;
; Include carry in Count (R0)
;
	addc	#0,Count	; R0 = R0 + 0 + C
;
; Repeat for next bit
;
	bra	BC_Loop
;
; Execute if zero:
; All done, use breakpoint to catch
;
Done	bra	Done
;
; Data to be read and counted
;
	org	#2000
Data	word	#FFFF
;
	end	Start