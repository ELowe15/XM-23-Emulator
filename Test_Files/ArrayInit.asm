;
; Sample X-Makina XM23 program
; Initialize a block of memory to 'A' through 'Z'
; ECED 3403
; 5 May 2023
;
SIZE	equ	$26
CAP_A	equ	'A'
CAP_Z	equ	'Z'
;
; Start of data area
;
	org	#800
Base	bss	SIZE		; Reserve SIZE bytes
	byte	'*'
;
; Start of code area
;
	org	#1000
;
Start	movlz	CAP_A,R0	; R0 = 'A'
	movlz	CAP_Z,R1	; R1 = 'Z'
; R2 = Base (Base address to store characters)
	movl	Base,R2		; R2 = LSByte(Base) or #00	
	movh	Base,R2		; R2 = MSByte(Base) or #08
; 
Loop
	st.b	R0,R2+		; [R2] = R0; R2 = R2 + 1
	cmp.b	R0,R1		; R0 = R1? ('Z')
	beq	Done		; Equal - leave
	add.b	$1,R0		; No: R0 = R0 + 1 (next ASCII char)
	bra	Loop		; Repeat loop
;
; End of program
;
Done	movlz	'*',R1
	bra	Done
;
	end	Start		; End of program - first executable address is "Start"
