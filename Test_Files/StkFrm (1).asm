;
; Stack frame example
; Call a subroutine that takes a variable number of parameters
;
; ECED 3403
; 22 Jun 2023 - Revised for XM23
;  5 May 2022 - XMX version
;
STK_START	equ	#FFC0	; Very high memory
LAST_SF		equ	#FFFF	; Signal last (i.e., first) Stk Frame
;
; Registers:
; R0 and R1 as data
; R2 and R3 as address
; R4 to R7 for CPU and subroutines
;
PC	equ	R7	; Program counter
SP	equ	R6	; Stack pointer
LR	equ	R5	; Link register
BP	equ	R4	; Base Pointer - Stack frame
;
; Data
;
	org	#80
Total	bss	#2		; Space for sum
;
; Code
;
	org	#1000
StkFrmEx			; Mainline/entry point
;
	movls	STK_START,SP	; Initialize SP (#FFC0; top is #FFBE)
	movls	LAST_SF,BP	; BP = #FFFF
;
; sum(Count, P1, P2, ... Pn)
; Count is number of parameters to count
; Returns total (sum) of parameters
;
; x = sum(4, 3, 4, 8, 2)
; 
; Push from right to left (C convention so count is at TOS)
;
	movlz	$2,R0		; Rightmost parameter
	st	R0,-SP
	movlz	$8,R0
	st	R0,-SP
	movlz	$4,R0
	st	R0,-SP
	movlz	$3,R0
	st	R0,-SP
	movlz	#4,R0		; Count (leftmost)
	st	R0,-SP
;
	bl	Sum		; Call
;
; Return - tidy up:
; - R0 has total
; - Stack must be returned to original top (5 words)
;
	movlz	Total,R2	; R2 <- address of Total (#0080)
	st	R0,R2		; mem[R2] (Total) <- R0
;
; Restore stack pointer
; Increment 5 words (10 bytes) to bypass parameters
; SP <- SP + 10
;
	movlz	$10,R0
	add	R0,SP
;
; Put brkpt here
;
Done
	bra	Done

;
; ****************************************************************************
;
	org	#2000
;
; Subroutine Sum
; sum(Count, P1, P2, ... Pn)
; Count is number of parameters to count
; Returns total (sum) of parameters
; Sum uses automatic 'i' to hold count
;
Sum
	st	BP,-SP		; SP decremented, then mem[SP] <- BP
	mov	SP,BP		; BP for this stkframe (mem[BP] is old BP)
;
; mem[BP] is old BP
; Decrement SP past old BP to area for automatics
; Only one automatic (i), decrement SP by 2 (mem[SP] is automatic)
; *** Next push will be -SP, decrement SP before accessing stack; 
; 'i' is unchanged
;
; SP <- SP - 2
;
	sub	#2,SP		; New tos
;
; i (automatic) <- count (parameter)
; mem[BP-2] <- mem[BP+2]
;
	ldr	BP,$2,R0	; R0 <- mem[BP+2]
	str	R0,BP,$-2	; mem[BP-2] <- R0
;
; R2 is to point to next value to add (mem[BP+4])
; R2 <- BP+4
	mov	BP,R2
	add	#4,R2		; Address of first data value
;
; R1 is total
;
	movlz	#0,R1		; R1 (total) is cleared
;
SumLoop
;
; Check 'i' ([mem[BP-2]) *** This is redundant, optimize with R0 alone ***
;
	ldr	BP,$-2,R0	; R0 <- 'i'
;
	cmp	#0,R0		; Check for end-of-loop
	beq	SumDone		; Leave loop when 'i' is zero
;
; Update 'i' (mem[BP-2]) 
; This would be redundant with more registers
; R0 (currently 'i') needed for addition
;
	sub	#1,R0		; i = i - 1
	str	R0,BP,$-2
;
	ld	R2+,R0		; Read next word (from [R2]) into R0
	add	R0,R1		; Add R0 to R1 (total)
;
	bra	SumLoop		; Repeat
;
SumDone
	mov	R1,R0		; Return total in R0
;
; Restore SP and caller's BP
;
	mov	BP,SP		; SP <- BP
	ld	SP+,BP		; SP points to first parameter
				; Old BP restored
;
	mov	LR,PC		; return
;
	end	StkFrmEx
