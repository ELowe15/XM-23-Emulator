
;
; Example X-Makina program
; Summing elements of an array
; Called by mainline
;
; L. Hughes and Evan Lowe
;
; July 9 2023 - 
;
;
; Code space
;
	org	#1000
Main
;
; R1 points to array
;
	movl	Array,R1
	movh	Array,R1
	movlz	#0,R0	; Return sum in R0 (initially zero)
;
; Call arrayCount()
; R1 points to array
; R0 returned with sum
;
	bl	arrayCount	
;
; Array sum function
; Input: R1 - Address of array
; Returns: sum of elements in R0
	org	#2000
arrayCount
	ld	R1+,R2	; Copy [R1] to R2 and R1 incremented by 1
	add	R2,R0	; Add elements of the array
	bn	Done	; If negative, go to Done
	bra	arrayCount	; Check next word
;
; Finished
;

Done	movlz	'*',R1
	bra	Done
;


; Data space 
; Setting the origin could be unsafe - if code addresses extend past $1000, they
; could be overwritten by the data.  This is one argument for a linker.
;
	org	#3002
;
Array	word	$1
	word	$32
	word	$24
	word	$-24
	word	$-33
	word	$-1

	end	Main		; End of program - first executable address is "Main"
