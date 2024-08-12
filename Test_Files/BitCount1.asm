;
; Bit count #1
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
; Check LSbit for 0
;
	bit	#0,DataVal
	beq	NotSet		; Bit is zero
;
; Bit not zero, clear it and add 1 to count
;
	bic	#0,DataVal
	add	#1,Count
;
NotSet
	rrc	DataVal		; Rotate DataVal right
;
; Repeat for next bit
;
	bra	BC_Loop
;
; Execute if zero:
;
Done	bra	Done		; Use breakpoint to catch
;
; Data to be read and counted
;
	org	#2000
Data	word	#FFFF
;
	end	Start
