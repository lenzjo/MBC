;=========================================================================
;====  PCF8584 FUNCTIONS  ====
;=========================================================================

;==== Iniz device  ====
pcf8584_iniz
			lda #CTRL_PIN							; own address reg S1
			sta P8584_S1
			lda #$55								; own address becomes $AA 
			sta P8584_SX
			lda #CTRL_PIN | #CTRL_ES1				; clk ctrl reg S2
			sta P8584_S1
			lda #$18								; $18 = 90kHz SCL, 8MHz input clock ($19 would be 45kHz SCL)
			sta P8584_SX
			lda #CTRL_PIN | #CTRL_ENI | #CTRL_ACK	; Enable serial, i2c is idle, SDA & SCL are HIGH
			sta P8584_S1
			rts

			

;====  Write a message  ====
MSG_OUT		= $300					; Addr of msg to be sent
MSGLEN		.BYTE					; Length of msg
PTR			.BYTE

i2c_write
			stz PTR
			
			jsr wait_until_free
			ldy PTR
			lda (MSG_OUT),y
			and #%11111110
			sta P8584_SX				; slave address
			inc PTR
			lda #$C5					; PIN + ENO + STA + ACK
			sta P8584_S1
i2cWrLoop
			lda P8584_S1	; get status
			bmi i2cWrLoop
			
			and #ST_AD0LRB
			bne i2cEndMsg
			ldy PTR
			cpy MSGLEN
			beq i2cEndMsg
			lda (MSG_OUT),y
			iny
			sty PTR
			sta P8584_SX	; si2cEndMsg next byte
			jmp i2cWrLoop
i2cEndMsg
			lda #$C3				; PIN + ENO + STO + ACK
			sta P8584_S1
			lda PTR
			cmp MSGLEN
			bne i2cWrErrExit
i2cWrGoodExit
			clc
			rts
i2cWrErrExit		
			lda #<I2C_ABORTSEND
			sec
			rts
			
			
			
			; receive a message from a device
			; a/x = lo/hi address of message.
			; first byte contains the slave address
			; slave address is not overwritten,
			; received message is stored at pointer+1
			; y = len of message (incl slave address)
			
;==== MSG_OUT = Msg buffer
i2c_read
			dey
			sty MSGLEN				; last valid index in MSG_OUT = # of data bytes
			stz PTR
			ldy PTR
			lda (MSG_OUT),y
			ora #%00000001			; set read bit
			sta P8584_SX				; slave address
			jsr wait_until_free
			lda #$C5					; PIN + ENO + STA + ACK
			sta P8584_S1
rloop
			lda P8584_S1	; get status
			bmi rloop
			and #P8584_ST_AD0LRB
			bne i2cErrExit				; error
			ldy PTR
			iny
			cpy MSGLEN
			beq lastbyte
			dey
			lda P8584_SX
			sta (MSG_OUT),y
			iny
			sty PTR
			jmp rloop
lastbyte
			dey
			lda #$40					; ESO + /ACK
			sta P8584_S1
			lda P8584_SX
			sta (MSG_OUT),y
			iny
lbloop		
			lda P8584_S1
			bmi lbloop
			lda #$C3
			sta P8584_S1
			lda P8584_SX				; final byte
			sta (MSG_OUT),y
i2cRdGoodExit
			clc
			rts
i2cErrExit
			lda #$C3					; PIN + ENO + STO + ACK
			sta P8584_S1
			lda #<I2C_ABORTREAD
			sec
			rts
	
;==== wait for bus free. check BB bit  ====
wait_until_free
			lda P8584_S1
			and #ST_BB
			beq wait_until_free
			rts
