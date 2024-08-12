;
; Example X-Makina program
; Assembling a string function (strlen() a
; Called by mainline
;
; L. Hughes and Evan Lowe
;
; July 9 2023 - 
;
NUL	equ	'\0'	; End-of-string indicator
; 
r2	equ	R2	; r2 is alias of R2
PC	equ	R7	; PC is alias of R7
;
; Code space
;
	org	#1000
Main
;
; R1 points to string
;
	movl	MyStr,R1
	movh	MyStr,R1
	movlz	#0,R0	; Return length in R0 (initially zero)
;
; Call strlen()
; R1 points to string
; R0 returned with length
;
	bl	strlen	
;
; String length function
; Input: R1 - Address of NUL-terminated string
; Returns: Length of string in R0
	org	#2000
strlen
	ld.b	R1+,R2	; Copy byte [R1] to R2 and R1 incremented by 1
	cmp.b	NUL,R2	; Compare byte (in R2) with NUL
	beq	Done	; If equal, go to Done
	add	#1,R0	; R0 (length) incremented by 1
	bra	strlen	; Check next byte
;
; Finished
;

Done	cmp	NUL,R0
;

; Data space 
; Setting the origin could be unsafe - if code addresses extend past $1000, they
; could be overwritten by the data.  This is one argument for a linker.
;
	org	#3002
;
MyStr	ascii	"The cat\n\0"
;
	end	Main