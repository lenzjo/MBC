.NOCODES
;=====================================================================
; assemble with tasm -65 -c  -b -x -f00 -a5 screen1.asm mon.bin
;=====================================================================
; PURPOSE: To start fresh with the ATMEGA328 video
; Working:	ATMEGA328 video
;			6522 VIA
;			6551 ACIA
;			Primitive STDIO functionality
;
; Commands:	<r>ead a byte from memory
;			<w>rite a byte to memory
;			<h>elp: Show Help Screen
;			<x>Screen: Toggle 40/80 column mode
;			<d>ump: Dump a block of memory
;			<k>lear screen and home cursor
;             <o>utput device change between Screen, Terminal or both
;			<m>emory edit easily. Cursor l/r is ,/. ESC exits program
;			- after any command, shows it's help message
;			<c>opy a block of memory
;			<f>ill a block of memory
;			<t>ext editor: Very basic - lets see what happens
;
; Plans:		1> I have a TMS9928 I would like to use instead of the '328,
;			though I am having difficulty finding the 10.738Mhz crystal
;			for it, I may have to use a 6545 instead :(
;			2> I have a PCF8584 waiting to be installed and setup, this
;			will add I2C capabilities.
;			3> I have a 16Mhz osc. to replace the 1Mhz I'm using at the
;			moment. With a divider (74LS293) I can then run at 1, 2, 4 
;			or 8Mhz. Might have to get faster memory...
;			4> I need a real keyboard!! I have an old C64 kybd that I
; 			might be able to use.
;			5> I want to add SPI, probably thru the 6522, so that I can
;			use an SD card for storage, and maybe a RT clock.
;			6> I also have some 22V10 GALs, one of those could replace
;			the three chips I am using now for decoding, but it means
;			virtually starting over and it works NOW!!!
;			7> Major plan is to produce a circuit board and the sooner
;			the better. Then I can make a case for her.
;
; Firmware:	This is the really fun part of the project! Right now, 
;			TLC-MBC hasn't really got an OS, just a collection of
;			functions that don't really do a lot. The text editor
;			will be the first 'real' application. I also need to
;			write a real device handler as I am adding more I/O it
;			becomes more difficult to utilise.
;
;=====================================================================
.CODES



.NOCODES
;=====================================================================
; Memory Map Constants
;=====================================================================
.CODES

; 62256 RAM - 32K
RAMBASE		= $0000
RAMLEN		= $7FFF

; 28256 EEPROM - 32K
ROMBASE		= $8000

.NOCODES
;=====================================================================
; Hardware Memory Map Constants
;=====================================================================
.CODES

#INCLUDE "hw/ACIA1_map.inc"
#INCLUDE "hw/VIA1_map.inc"


.NOCODES
;=====================================================================
; CHARACTER CONSTANTS
;=====================================================================
.CODES

#INCLUDE "ascii_con.inc"

.NOCODES
;=====================================================================
; PROGRAM CONSTANTS
;=====================================================================
.CODES


MAXBUFF		= $80				; Max length of CMDBUFF

HIGH			= $01
LOW			= $00

#INCLUDE "hw/screen_con.inc"

#INCLUDE "device_con.inc"

#INCLUDE "hw/ACIA_con.inc"

.NOCODES
;=====================================================================
; ZERO PAGE REGS
;=====================================================================
.CODES

ZPTR			= $30
MSG_PTR		= ZPTR 				; Ptr for Pr_Str

SRC			= ZPTR+2				; Start addr
MEMLEN		= ZPTR+4				; length of mem to dump, move or copy
BYTECNT		= ZPTR+6				; Val depends on screen width

BDATA		= ZPTR+7
ASCBYTE		= ZPTR+8
HEX_RES		= ZPTR+10

ADFROM		= ZPTR+12			; Used by Copy and move functions
ADUNTIL		= ZPTR+14
ADTO			= ZPTR+16
XYREG		= ZPTR+18


;====  STARTS AT $50  ====
#INCLUDE "device_zpr.inc"

;====  STARTS AT $60  ====
#INCLUDE "hw/screen_zpr.inc"

;====  STARTS AT $70  ====
#INCLUDE "cmdline_zpr.inc"

TEMP			= $F0

CMDBUFF		= $0200				; Input Buffer

ASCIIDUMP	= $02C0				;  16 bytes used by dump 

.NOCODES
;=====================================================================
; My MACROS
;=====================================================================
.CODES

#INCLUDE "macros.inc"

.NOCODES
;=====================================================================
; UNUSED ROM: $8000-$83FF is used by the I/O
;=====================================================================
.CODES
			
			*= ROMBASE
			.NOLIST
			.FILL $400, $FF		; Fill the first k with NOPs
			.LIST
			
RST_VEC		jmp Main

NMI_VEC		rti

IRQ_VEC		rti

.NOCODES
;=====================================================================
; Cold reset
;=====================================================================
.CODES

Main
			ldx #$FF				; Iniz SP to $01FF
			txs
			
			jsr ClrZPage			; Fill Zero Page with $00
			jsr InizACIA			; Setup the 6551
			jsr InizVIA			; Setup the 6522
			
			lda #$00				; Iniz regs to zero
			tax
			tay
			cld					; Clear decimal mode
			clc					; Clear the carry
			cli					; Enable interrupts
MonStart
			jsr iniz_IOdevices	; Setup default I/O
			
			jsr scr_clear			; Clear screen
			jsr scr_40col			; Set to 40 col screen
			jsr Pr_SysHdr			; and print header
cmdLoop			
			jsr Pr_Prompt			; Show a prompt
			jsr GetCmd
			cpx #$00				; Empty cmd line?
			beq cmdLoop			; Yes, then go get another
			
			jsr ParseCmd			; Set ptrs to ARGs
			bcs cmdR
			jsr Pr_Encmd			; Print Invalid cmd
			bra cmdLoop			; and try again
			
			;==============================================
			;====  Each cmd is identified now by a number,
			;====  not a letter as before,
			;====  as I'm thinking about using a jump table
			;====  for all system cmds.
			;==============================================
cmdR
			cpy #$00
			bne cmdW				; Not a Read a byte
			jsr read_byte
			bra cmdLoop
cmdW
			cpy #$01
			bne cmdD				; It's not a Write a byte to an address
			jsr write_byte
			bra cmdLoop
cmdD
			cpy #$02
			bne cmdC				; It's not a Dump memory block
			jsr dump_memblk
			bra cmdLoop
cmdC
			cpy #$03
			bne cmdF				; It's not a Copy memory block
			jsr copy_block
			bra cmdLoop
cmdF
			cpy #$04
			bne cmdK				; It's not a Fill memory block
			jsr fill_block
			bra cmdLoop
cmdK
			cpy #$05
			bne cmdX				; It's not a screen clear
			jsr screen_clear
			bra cmdLoop
cmdX
			cpy #$06
			bne cmdH				; Not Screen width
			jsr toggle_scrwid
			bra cmdLoop
cmdH
			cpy #$07
			bne cmdO				; Not Help
			jsr Pr_AllHelp
			bra cmdLoop
cmdO
			cpy #$08
			bne cmdM				; It's not a change STDIO device
			jsr change_stdio
			bra cmdLoop
cmdM
			cpy #$09
			bne cmdE				; It's not a Memory edit
			jsr edit_mem
			bra cmdLoop
cmdE
			cpy #$0A
			bne cmdErr			; It's not the Text Editor
			jsr text_edit
			bra cmdLoop
cmdErr
			jsr Pr_Evcmd			; Err - not a cmd
			bra cmdLoop			; Get next command
			
.NOCODES
;=====================================================================
; SYSTEM COMMAND FUNCTIONS
;=====================================================================
.CODES

#INCLUDE "cmds/cmd_help.inc"
#INCLUDE "cmds/cmd_scrwid.inc"
#INCLUDE "cmds/cmd_readbyte.inc"
#INCLUDE "cmds/cmd_writebyte.inc"
#INCLUDE "cmds/cmd_dump.inc"
#INCLUDE "cmds/cmd_stdout_dev.inc"
#INCLUDE "cmds/cmd_scrclr.inc"
#INCLUDE "cmds/cmd_editmem.inc"
#INCLUDE "cmds/cmd_copyblock.inc"
#INCLUDE "cmds/cmd_fillblock.inc"
#INCLUDE "cmds/cmd_texted.inc"


.NOCODES
;=====================================================================
; SYSTEM FUNCTIONS
;=====================================================================
.CODES

;====  List of valid commands  ====
VALID_L_CMDS	.BYTE "rwdcfkxhomt", NULL
;==== or full words...
VALID_W_CMDS	.BYTE "read", NULL, "write", NULL, "dump", NULL, "copy", NULL, "fill", NULL
			.BYTE "scrclr", NULL, "toggle", NULL, "help", NULL, "stdout", NULL, "edit", NULL

;==== On Exit    : THISCMD | Carry |  ARGCNT  |  ARG1-ARG5
;==== No cmds    :   NULL  |  CLC  |    0     |    0
;==== cmd present: letter  |  SEC  |# of args | index to arg
;==== If cmd present Y = index, else Y=#$FF
ParseCmd
			ldx #$00				; Line-ptr
			ldy #$00				; cmd ptr
			stx ARGCNT			; Clear the arg ctr
			stx THISCMD			; and curr cmd
pcCmdSrch						; Find cmd loop
			lda CMDBUFF,x
			cmp VALID_L_CMDS,y	; Is input cmd in list?
			beq pcCmdFnd			; found it!
			iny
			lda VALID_L_CMDS,y
			cmp #NULL			; End of List?
			bne pcCmdSrch			; No, go get next cmd
			
			; now check VALID_W_CMDS
			
			sta THISCMD			; <<== not really needed!!!!!
			ldy #$FF
			clc
			rts
pcCmdFnd
			sta THISCMD			; Save cmd
			phy					; and cmd ptr
findArg1
			stz ARG1
			jsr FindNxtSPC
			bcc pcExit			; No args
			jsr FindNextArg
			bcc pcExit			; No Args
			stx ARG1				;  ptr to first arg
			inc ARGCNT
findArg2
			stz ARG2
			jsr FindNxtSPC
			bcc pcExit			; No args
			jsr FindNextArg
			bcc pcExit			; No Args
			stx ARG2				;  ptr to first arg
			inc ARGCNT
findArg3
			stz ARG3
			jsr FindNxtSPC
			bcc pcExit			; No args
			jsr FindNextArg	
			bcc pcExit			; No Args
			stx ARG3				;  ptr to first arg
			inc ARGCNT
findArg4
			stz ARG4
			jsr FindNxtSPC
			bcc pcExit			; No args
			jsr FindNextArg
			bcc pcExit			; No Args
			stx ARG4				;  ptr to first arg
			inc ARGCNT
findArg5
			stz ARG5
			jsr FindNxtSPC
			bcc pcExit			; No args
			jsr FindNextArg
			bcc pcExit			; No Args
			stx ARG5				;  ptr to first arg
			inc ARGCNT
pcExit	
			lda THISCMD			; restore cmd
			ply					; and cmd ptr
			sec
			rts

		
;========== On Exit: SEC = SPC found, CLC = End of line ===============
FindNxtSPC
			inx
			lda CMDBUFF,x
			beq fs_EOL			; It's a NULL!
			cmp #SPC
			beq fs_SPC			; It's a SPC
			bra FindNxtSPC		; Neither so loop
fs_EOL
			clc
			rts
fs_SPC
			sec
			rts

			
;========== On Exit: SEC = cmd found, CLC = End of line ===============
FindNextArg
			inx
			lda CMDBUFF,x			; Get next char
			beq Eol_Exit			; Exit if a NULL
			
			cmp #SPC
			beq FindNextArg		; Skip over spaces
cmdFound
			sec
			rts
Eol_Exit
			clc
			rts
	

;====  Clear Zero Page  ====
ClrZPage
			ldx #$FF				; byte count
			lda #$00				; fill byte
clrLoop
			sta $00,x			; fill mem
			dex
			bne clrLoop			; go to next addr
			sta $00,x			; clear last byte
			rts
			
;====  Read a line from the STDIN device  ====
GetCmd
			ldx #$00				; CMDBUFF chr ptr
gsLoop
			jsr GetC				; Read a character
			cmp #CR
			beq gsExit			; If it's a CR then exit
			cmp #BS
			bne gsLp1			; If it's not a BackSpace go on
			jsr BackSpc			; Do the BS
			jmp gsLoop			; and loop
gsLp1
			cmp #DEL
			bne gsLp2			; If it's not the Delete Key go on
gsDel1
			jsr BackSpc
			cpx #$00				; Delete everything from the buffer
			bne gsDel1
			beq gsLoop			; and start over
gsLp2
			cpx #MAXBUFF
			bcc gsSvChr			; If CMDBUFF not full jump
			lda #BELL
			jsr PutC				; sound the bell
			jmp gsLoop			; and loop
gsSvChr
			cpx #$00				; Is it 1st char?
			beq gsSvChr2			; Yes then go save it
;			cmp #$60				; Is it lower case?
;			bcc gsSvChr2			; Yes, then go
;			and #$DF				; convert to upper case
gsSvChr2
			sta CMDBUFF,x			; Save the char
			jsr PutC				; Print it
			inx
			jmp gsLoop			; and loop
gsExit
			jsr Pr_Crlf
			stz CMDBUFF,x			; Put a NULL at end of cmd string
			rts
			
;====  Destructive BackSpace  ====
BackSpc
			cpx #NULL			; If no chars
			beq bsExit			; then exit
				
			dex
			lda #BS
			jsr PutC
			jsr Pr_SPC
			lda #BS
			jsr PutC
bsExit
			rts
			
	
.NOCODES
;=====================================================================
; CHARACTER DECODING & ENCODING
;=====================================================================
.CODES

;====  General function to decode a number from the cmd line  ==
;====  If prefixed by a $ it is a hex number, otherwise it is decimal  ====
GetASCnum
			lda CMDBUFF,x			; Get 1st char of number
			cmp #'$'				; Is it Hex
			bne ganIsDec			; No, then go get decimal #
			jsr GetASChex
			rts
ganIsDec
			jsr GetASCdec
			rts
			
;==== Convert an ASCII decimal number on the cmd line to binary  ====
GetASCdec
			stz HEX_RES
			stz HEX_RES+1
next_digit

			lda CMDBUFF,x			; Get a chr
			cmp #SPC				; End of #?
			beq b2hGoodExit		; Yes, then exit
			cmp #NULL			; End of cmd?
			beq b2hGoodExit		; Yes, then exit
			cmp #'0'				; Is it numeric?
			bmi b2hErrExit		; No, then err exit
			cmp #'9'+1
			bpl b2hErrExit		; No, then err exit
b2h_cont
			and #$0F				; convert to binary
			pha					; Save temporarily
			
			lda HEX_RES			; Save current result
			sta TEMP
			lda HEX_RES+1
			sta TEMP+1
			; This bit multiplies the curr result by 10
			MULTW2(HEX_RES)		; Multiply by 2
			MULTW2(HEX_RES)		; Multiply by 2 again
			ADDW(HEX_RES,TEMP)	; Add original #w 
			MULTW2(HEX_RES)		; Multiply by 2

			pla					; Get new #
			clc					; and add it to the last result
			adc HEX_RES
			sta HEX_RES
			lda #$00
			adc HEX_RES+1
			sta HEX_RES+1
			inx					; inc line ptr
			jmp next_digit
b2hGoodExit
			clc					; clc = success
			rts
b2hErrExit
			sec					; sec = failure
			rts
			
			
;====  Convert an ASCII Hex number on the cmd line to binary  ====
GetASChex
			stz HEX_RES			; clear the result regs
			stz HEX_RES+1
gahLoop
			inx					; inc line ptr
			lda CMDBUFF,x			; Get a chr
			cmp #SPC				; End of #?
			beq gahGoodExit		; Yes, then exit
			cmp #NULL			; End of cmd?
			beq gahGoodExit		; Yes, then exit
			
			jsr GetAscNybl		; Is it valid # ?
			bcs gahErrExit		; No, go to err exit
			MULTW2(HEX_RES)		; Yes, so shift left the result 4 times
			MULTW2(HEX_RES)
			MULTW2(HEX_RES)
			MULTW2(HEX_RES)
			
			ora HEX_RES			; add in the new nybble
			sta HEX_RES
			bra gahLoop
gahGoodExit
			clc					; clc = success
			rts
gahErrExit
			sec					; sec = failure
			rts
			
			
;====  ASCII from line buffer to hex at temp. X ptr in line buffer  ====
GetASC_Word
			jsr GetASC_Byte		; Get HiByte
			bcs gawErr			; Not valid Hex
			sta BDATA			; save it
			inx					; Inc line ptr
			jsr GetASC_Byte		; Get LoByte
			bcs gawErr			; Not valid Hex
			tay					; LoByte in Y
			lda BDATA			; HiByte in A
			clc					; clc = success
			rts
gawErr
			sec					; sec = failure
			rts

			
;====  ASCII from line buffer to hex at temp. X ptr in line buffer  ====	
GetASC_Byte
			lda CMDBUFF,x			; get 1st char
			jsr GetAscNybl
			bcs gabErrExit		; No, go to err exit
			asl a
			asl a
			asl a
			asl a
			sta TEMP				; Save it
gabN2
			inx
			lda CMDBUFF,x			; get 2nd char
			jsr GetAscNybl		; get nibble
			bcs gabErrExit		; Not hex - exit
			ora TEMP				; result in A
gabExit	
			clc					; success exit
			rts			
gabErrExit
			sec					; failure exit
			rts

			
;====  Conv ASC to hex nibble. On Exit: SEC = ERR, clc = Valid  ====
GetAscNybl
			cmp #$30				; '0'
			bcc gnErr			; < '0'
			cmp #$47
			bcs ganlc			; > 'F'
			bra ganGd
ganlc
			cmp #$61				; is it lower case?
			bcc gnErr			; < 'a'
			cmp #$67
			bcs gnErr			; > 'f'
			and #$DF				; Convert to Upper Case
ganGd
			cmp #$3A
			bcs gnaf2nib			;  in range 0-9
			and #$0F
			bra gnGood
gnaf2nib
			cmp #$41
			bcc gnErr			; not in range A-F
			sbc #$37
gnGood
			clc					; Is a Hex char.
			rts
gnErr
			sec					; Is not Hex
			rts
	
			
Pr_HexByte	; Print byte in A from ASCBYTE
			jsr Byt2ASC			; Convert byte to ASCII
			lda ASCBYTE			; get Hi-nyb
			jsr PutC				; and print it
			lda ASCBYTE+1			; Get Lo-nyb
			jsr PutC				; and print it
			rts

			
Byt2ASC		; Convert a byte to an ASCII char in ASCBYTE & ASCBYTE+1
			pha					; Save byte
			lsr a
			lsr a
			lsr a
			lsr a				; move to lower nybble
			jsr Nyb2ASC			; convert to ASCII
			sta ASCBYTE			; and store it
			pla					; get the byte back
			and #$0F				; get lower nybble
			jsr Nyb2ASC			; convert to ASCII
			sta ASCBYTE+1			; and store it
			rts
			
			
Nyb2ASC		; Convert a nybble to an ASCII char in A
			clc
			adc #$30
			cmp #$3A				; in range 0-9?
			bcc b2aGood			; yes, then exit
			adc #$06				; must be A-F
b2aGood
			rts
	
	
			
.NOCODES
;=====================================================================
; SYSTEM MESSAGES
;=====================================================================
.CODES

;====               "1234567890123456789012345678901234567890" = 40 cols
;====               "12345678901234567890123456789012345678901234567890123456789012345678901234567890" = 80 cols
SYS_HDR		.BYTE "TLC-MBC Mon v1.12 - 25/07/15 (c)TLC", NULL
SYS_HDR2		.BYTE "32K Ram, 32468 bytes free.", NULL



.NOCODES
;=====================================================================
; Misc Print routines
;=====================================================================
.CODES

Pr_SysHdr
			CPYMSPTR(SYS_HDR)
			jsr Pr_Str
			jsr Pr_Crlf
			CPYMSPTR(SYS_HDR2)
			jsr Pr_Str
			jsr Pr_Crlf
			jsr Pr_Crlf
			rts
			
Pr_Prompt
			lda #'>'				; Show a prompt
			jsr PutC
			rts
			
Pr_Str
			sta MSG_PTR
			sty MSG_PTR+1
			ldy #$00
psLoop
			lda (MSG_PTR),y		; Get the next char in the string
			beq psExit			; NULL means the end
			jsr PutC				; Otherwise put the char
			iny					; Increment MSGPTR-lo
			bne psLoop			; No roll-over? Loop back for next character
			inc MSG_PTR+1			; MSGPTR-lo rolled over--carry hi byte
			jmp psLoop			; Now loop back
psExit
			rts
			
Pr_Crlf
			lda #CR
			jsr PutC
			lda #LF
			jsr PutC
			rts
			
Pr_SPC
			lda #SPC
			jsr PutC
			rts
			
			
.NOCODES
;=====================================================================
; SYSTEM ERROR MESSAGES & PRINT FUNCTIONS
;=====================================================================
.CODES

#INCLUDE "syserror_msg.inc"


.NOCODES
;=====================================================================
; ACIA routines
;=====================================================================
.CODES

InizACIA
			sei					; Disable ints.
			lda $00
			sta ACIA_STAT			; Reset the ACIA
			lda #%00001011		; No parity, no echo, no interrupt
			sta ACIA_COM
			lda #%00011111		; 1 Stop bit, 8 data bits, 19.2K baud
			sta ACIA_CTRL
			cli					; Enable the ints
			rts
			
			
term_getc	; Read one char from ACIA
			lda #ACIA_RXF
tgcLoop
			bit ACIA_STAT			; Get Status
			beq tgcLoop			; not yet
			lda ACIA_DATA			; read char
			rts

;============  Write one char to ACIA  ================================
term_putc	
			sta ACIA_DATA			; and write it
			jsr WAIT_6551			; required Delay
			rts
			
CLS			.BYTE ESC, "[2J", ESC, "[H", NULL

term_clear
			CPYMSPTR(CLS)
			jsr Pr_Str
			rts
			


WAIT_6551
			phy
			phx
W6_LOOP
			ldy #01				; Get Delay val (Clk rate in MHZ - 2 clk cycles)
MSEC
			ldx #$9C				; Seed X for 1ms
W6_DELAY
			dex
			bne W6_DELAY
			dey
			bne MSEC
			plx
			ply
			rts

			
.NOCODES
;=====================================================================
; VIA SETUP
;=====================================================================
.CODES

InizVIA
			lda #$FF				; Screen output data byte
			sta VIA_DDRA			; Make Port A output
			
			lda #AVAIL_BIT		; Waste a Port for 2 bits for handshaking
			sta VIA_DDRB			; Bit 0 is AVAIL (out), bit 1 is ACK (in)
			
			stz VIA_ORA			; Clear Port A
			stz VIA_ORB			; set AVAIL low
			
			lda #LOW				; then set AVAIL reg Low
			sta AVAIL_REG

			rts
		
.NOCODES		
;=====================================================================
;====  7" screen Functions  ====
;=====================================================================
.CODES

#INCLUDE "hw/screentxt_fun.inc"

.NOCODES		
;=====================================================================
;====  I/O Decision Stuff  ====
;=====================================================================
.CODES

#INCLUDE "io_fun.inc"
		
.NOCODES
;=====================================================================
; Set the interrupt vectors for 65C02 system 
;=====================================================================
.CODES

			*=  $FFFA 
			.word	NMI_VEC  	; NMI_VECTOR   : $FFFA  (NMI) 
			.word	RST_VEC      ; RESET_VECTOR : $FFFC  (Reset) 
			.word	IRQ_VEC  	; IRQ_VECTOR   : $FFFE  (IRQ) 

		.END