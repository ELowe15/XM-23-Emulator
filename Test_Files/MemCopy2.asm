;
; Copying one memory location to another
; Need two registers: address and holding register
; Using single address and indexed addressing
; ECED 3403
; 10 May 23
;
	org	#800		; Start at #0800
InVal	word	#FEDC		; Word initializes 2 bytes to value
				; #FEDC in locations #0800 and #0801
				; InVal has address #0800
	bss	#2		; BSS with symbol OutVal (address #0802)
				; 2 bytes, locations #0802 and #0803
;
	org	#1000		; Start at #1000
;
; R0 <- InVal (Address)
;
Main	movl	InVal,R0
	movh	InVal,R0
;
; R1 <- contents of InVal (R0)
; Indexed addressing
; Increment R0 after load has finished
;
	ld	R0+,R1		; R1 <- mem[R0]; R0 <- R0 + 2 (#0800 + #0002)
;
; R0 now refers to location #0802 (OutVal)
; Location R0 <- contents of R1
;
	st	R1,R0		; mem[R0] <- R1
;
; All done
;
	end	Main		; PC <- address of Main on startup
