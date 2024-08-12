;
; Sample X-Makina XM23 program
; Test SETCC and CLRCC
; ECED 3403
; July 24 2023
;
; Start of code area
;
	org	#1000
;
Start	setcc	c
	setcc	c
	setcc	v
	clrcc	c
	clrcc	c
	setcc	s
	setcc	n
	setcc	z
	clrcc	v
	clrcc	s
	clrcc	n
	clrcc	z
	setcc	czn
	clrcc	zsv
	setcc	cvsnz
	clrcc	sn
	
;
; End of program
;
Done	movlz	'*',R1
	bra	Done
;
	end	Start		; End of program - first executable address is "Start"
