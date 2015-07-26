;============================================================================
;====  Copy a block of memory  ====
;============================================================================


fill_block
			; Check that all args are present
			ldx ARG1				; Get first arg
			bne fbIsHelp			; found
			jsr Pr_Efaddrm		; Print error msg
			rts
fbIsHelp	
			lda CMDBUFF,x
			cmp #'?'				; want help?
			bne fbCheckArg2		; No, go test if addr
			jsr Pr_Fhelp			; show cmd help msg
			rts
fbCheckArg2
			ldx ARG2				; Have we got an Until addr?
			bne fbCheckArg3
			jsr Pr_Euaddrm
			rts
fbCheckArg3
			ldx ARG3				; Have we got a data byte?
			bne fbGetFAddr
			jsr Pr_Edatam
			rts
			;==== Now check that addrs etc are valid
fbGetFAddr
			ldx ARG1
			jsr GetASCnum			; Get From Address
			bcc fbGotUaddr		; Is Valid
			jsr Pr_Efaddri		; Err msg
			rts
fbGotUaddr
			CPYW(HEX_RES,ADFROM)	; Store 'from' addr
			
			ldx ARG2
			jsr GetASCnum			; Get Until Address
			bcc fbGotTaddr		; Is Valid
			jsr Pr_Euaddri		; Err msg
			rts
fbGotTaddr
			CPYW(HEX_RES,ADUNTIL)	; Store 'until' addr
			
			ldx ARG3
			jsr GetASCnum			; Get data byte
			bcc fbGotAinfo		; Is Valid
			jsr Pr_Edatai			; Err msg
			rts
fbGotAinfo
			CPYW(HEX_RES,BDATA)	; Store the fill byte
			
fill_block_main
			sty XYREG			; save Y
			ldy #$00
fb_loop
			CMPFR2UN				; cmp
			bcc fb_cont			; from < until so go continue
			ldy XYREG			; restore Y
			rts
fb_cont
			lda BDATA			; Get the data byte
			sta (ADFROM),y		; and fill the curr addr
			INCW(ADFROM)			; inc the addr
			bra fb_loop			; and go to the next location








