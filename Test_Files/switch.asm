;
; switch.asm
; Example of switch() statement implementation
; See section 13.2.2 of XM23 ISA document
;
; The following switch() statement example uses a table of case-labels
; to decide where to pass control (i.e., which label).
;
; The subroutine CaseOffset() is called with a pointer to a CaseTable 
; structure, with fields and offsets as follows:
; Contents:					Type:	Offset:
; Lower limit of CaseTable			WORD	+0
; Upper limit of CaseTable			WORD	+2
; Default Address				WORD	+4
; First Case address in CaseTable array	WORD	+6
; ...
; Last Case address in CaseTable array	WORD	
;
; The sample switch statement:
; switch(x)
; {
; case 10: /* 'A' */
; break;
; case 11:
; case 15: /* 'B' */
; break;
; case 12: /* 'C' */
; break;
; default: /* 'D' */
; }
;
; L. Hughes
;  1 Jun 2023 - Replace CEX and XMX inst. with XM23 inst. 
;  3 Apr 2022 - Upgrade to XMX inst.
; 24 Mar 2020 - Replaced cond. branches with CEX
; 16 Apr 2019 - Upgraded to XM2
; 30 May 2018 - Original
;
; Useful equates:
; Registers:
;
LR	equ	R5
SP	equ	R6
PC	equ	R7
;
TOS	equ	#FFC0	; Highest location for stack
;
	org	#1000
;
; Start of program
;
Mainline
;
; Initialize SP
;
	movls	TOS,SP
;
; Sample switch(x)
; R0 contains 'x', the switch expression.  In this example, the lower bound is
; 10 and the upper bound is 15.
;
;*******************************************************
;* For testing, R0 should be changed by the debugger   *
;*******************************************************
;
	movl	CaseTable,R4	; R4 points to CaseTable
	movh	CaseTable,R4
	bl	CaseOffset	; call CaseOffset()
;
; R1 is returned from CaseOffset(), it contains the address of the case
; label
;
	mov	R1,PC		; PC = R1 -- Control passed with address in R1
;
	org	#1100
Case10
	; Case 10 instructions
	movlz	'A',R2
	bra	CaseExit	; break
;
	org	#1200
Case11
Case15
	; Case 11 and 15 instructions
	movlz	'B',R2
	bra	CaseExit	; break
;
	org	#1300
Case12
	; Case 12 instructions
	movlz	'C',R2
	bra	CaseExit	; break
;
	org	#1400
Default
	; Default instructions
	movlz	'D',R2
	; End of switch, next instruction follows '}'
;
CaseExit
;
; Check R1 for case-address and R2 for character associated with address
;
Wait
	bra	Wait
;
;	Rest of mainline...
;
; ***********************************************************************
;
	org	#2000
;
; Case label table holding case table limits and label addresses. Different 
; switch statements would have different expressions, lower and upper limits,
; and Case label addresses.  Case label addresses are the address of 
; each case label (see above and .LIS file)
;
CaseTable
	word	$10	 	; Lower bound		(+0)
	word	$16	 	; Upper bound+1	(+2)
	word	Default	; Default address	(+4)
; List of case label addresses
	word	Case10		; 10			(+6)
	word	Case11		; 11
	word	Case12		; 12
	word	Default	; 13
	word	Default	; 14
	word	Case15		; 15
	word 	Default	; Catch-all
;
; ***********************************************************************
;
	org	#3000
;
; Equated values for access to case table - note these are all 
; offsets into CaseTable (i.e., relative addresses)
;
LB	equ	$0	; Offset for lower-bound
UB	equ	$2	; Offset for upper-bound
DEF	equ	$4	; Offset for default (last in list)
LIST	equ	$6	; First label in list
;
; Entry point to Case offset calculator 
CaseOffset
;
; R0 is expression
; R1 is address of case table structure
; R2 is scratch register (undefined after call)
; Return R1 as address of case label
;
; 1. Compare with lower and upper limits
; Check lower bound
;
	ldr	R4,LB,R2	; R2 = mem[R4+LB] (lower bound)
	cmp	R2,R0		; tmp = R0 - R2 *** Note: DST (R0) - SRC (R2)
	blt	CaseOffOOR	; R0 < LowerBound - out of range
;
; Check upper bound
;
	ldr	R4,UB,R2	; R2 = mem[R4+UB] (upper bound)
	cmp	R2,R0		; tmp = R0 - R2 *** Note: DST (R0) - SRC (R2)
	blt	CaseOffIR	; R0 < UpperBound - in range; else out-of range
;
; 2. Case offset is out of range - use default
;
CaseOffOOR
	ldr	R4,DEF,R1	; R1 = mem[R4+DEF]
	mov	LR,PC		; return R1 as Default case address
;
CaseOffIR
;
; 3. Case offset is in range: R0 >= LB and R0 < UB (10..15)
;
; 'x' (R0) is between LB (10) and UB (15)
; Normalize R0 to 0 by subtracting it from the LB (10), giving 0..5
;
	ldr	R4,LB,R2	; R2 = mem[R1+LB] (lower bound)
	sub	R2,R0		; R0 = R0 - R2
;
; R0's value is 0..5
; Multiply R0 by 2 (shift left by 1) to get word offset 
; (0, 2, 4, 6, 8, or 10). Adding R0 to itself is functionally equivalent 
; to multiplying by 2 
;
	add.w	R0,R0
;
; Pass control to specified address:
; a) Get base address of Case labels (LIST) from the CaseTable (R1)
;
	movlz	LIST,R1		; R1 = LIST (#6)
	add	R4,R1		; R1 = R4 (address of CaseTable) + R1 (LIST)
;
; b) Add R0 (x * 2) to the base address (R1) to get address of 
; target Case 
;
	add	R0,R1		; R1 = R1 + R0
;
; c) R1 is the address of the location holding the Case label address
;
	ld	R1,R1		; R1 = [R1]
;
; d) Return target Case label address in R1
;
	mov	LR,PC		; Return
;
	end	Mainline	; Starting address

