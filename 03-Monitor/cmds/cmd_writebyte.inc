
;==== Write a byte to an address ====			
write_byte
			; Check that all args are present
			ldx ARG1				; Get first arg
			bne wbIsHelp			; found
			jsr Pr_Esaddrm		; Print error msg
			rts
wbIsHelp	
			lda CMDBUFF,x
			cmp #'?'				; want help?
			bne wbarg2			; No, go test for ARG2
			jsr Pr_Whelp			; show cmd help msg
			rts
wbarg2
			ldx ARG2				; Get second arg
			bne wbGetAddr			; found
			jsr Pr_Edatam			; Print error msg
			rts
			
			; Args present so get 'em
wbGetAddr
			ldx ARG1				; get line ptr
			jsr GetASCnum			;GetASC_Word
			bcc wbgAddr			; Is Valid
			jsr Pr_Esaddri		; Err msg
			rts
wbgAddr
			lda HEX_RES+1
			sta SRC+1
			lda HEX_RES
			sta SRC
			
			; Now get data byte
			ldx ARG2
			jsr GetASCnum			; GetASC_Byte
			bcc wbgb1			; found valid data byte
			jsr Pr_Edatai			; Err msg
			rts

			; Addr in SRC data in A - poke it
wbgb1
			lda HEX_RES
			sta (SRC)
			rts
			