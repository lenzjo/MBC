;============================================================================
;====  Copy a block of memory  ====
;============================================================================


copy_block
			; Check that all args are present
			ldx ARG1				; Get first arg
			bne cbIsHelp			; found
			jsr Pr_Efaddrm		; Print error msg
			rts
cbIsHelp	
			lda CMDBUFF,x
			cmp #'?'				; want help?
			bne cbCheckArg2		; No, go test if addr
			jsr Pr_Chelp			; show cmd help msg
			rts
cbCheckArg2
			ldx ARG2				; Have we got an Until addr?
			bne cbCheckArg3
			jsr Pr_Euaddrm
			rts
cbCheckArg3
			ldx ARG3				; Have we got a To addr
			bne cbGetFAddr
			jsr Pr_Etaddrm
			rts
			;==== Now check that addrs are valid
cbGetFAddr
			ldx ARG1
			jsr GetASCnum			; Get From Address
			bcc cbGotUaddr		; Is Valid
			jsr Pr_Efaddri		; Err msg
			rts
cbGotUaddr
			CPYW(HEX_RES,ADFROM)	; Store 'from' addr
			
			ldx ARG2
			jsr GetASCnum			; Get From Address
			bcc cbGotTaddr		; Is Valid
			jsr Pr_Euaddri		; Err msg
			rts
cbGotTaddr
			CPYW(HEX_RES,ADUNTIL)	; Store 'until' addr
			
			ldx ARG3
			jsr GetASCnum			; Get From Address
			bcc cbGotAaddr		; Is Valid
			jsr Pr_Euaddri		; Err msg
			rts
cbGotAaddr
			CPYW(HEX_RES,ADTO)	; Store 'until' addr
			
copy_block_main
			lda ADFROM+1			; copy to higher or lower addr?
			cmp ADTO+1
			bne cb_1
			lda ADFROM
			cmp ADTO
cb_1
			bcc copy_hi
copy_lo
			sty XYREG			; save X
			ldy #$00
cl_loop
			CMPFR2UN				; cmp
			bcc cl_cont			; from < until so go continue
			ldy XYREG			; restore X
			rts
cl_cont
			lda (ADFROM),y		; copy a byte
			sta (ADTO),y
			INCW(ADFROM)			; and inc from and to address's
			INCW(ADTO)
			bra cl_loop			; go to the next addr
			
copy_hi
			sty XYREG			; save X
			ldy #$00
			SUBW(ADTO,ADFROM)		; Do some math ;)
			ADDW(ADTO,ADUNTIL)
ch_loop
			CMPFR2UN				; cmp
			bcc ch_cont			; if from < until then go continue
			ldy XYREG
			rts
ch_cont
			DECW(ADUNTIL)
			DECW(ADTO)
			lda (ADUNTIL),y
			sta (ADTO),y
			bra ch_loop
			