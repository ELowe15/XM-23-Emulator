;
; Copying one memory location to another
; Need two registers: source/destination address and holding register
; ECED 3403
; 10 May 23
;
	org	#800		; Start at #0800
InVal	word	#FEDC		; Word initializes 2 bytes to value
				; #FEDC in locations #0800 and #0801
				; InVal has address #0800
OutVal	bss	#2		; BSS with symbol OutVal (address #0802)
				; 2 bytes, locations #0802 and #0803
;
	org	#1000		; Start at #1000
;
; R0 <- InVal (Address)
;
Main	movl	InVal,R0
	movh	InVal,R0
;
; R1 <- contents of InVal
;
	ld	R0,R1		; R1 <- mem[R0]
;
; R0 <- OutVal (Address)
;
	movl	OutVal,R0
	movh	OutVal,R0
;
; Location OutVal <- contents of R1
;
	st	R1,R0		; mem[R0] <- R1
;
; All done
;
	end	Main		; PC <- address of Main on startup
