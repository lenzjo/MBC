
;====  Move an address into regs A & Y  ====
#DEFINE	CPYMSPTR(ptr)			lda #<ptr
#DEFCONT					\	ldy #>ptr


;==== Copy a Word
#DEFINE CPYW(worda,wordb)		lda worda
#DEFCONT 				\	sta wordb
#DEFCONT 				\	lda worda+1
#DEFCONT 				\	sta wordb+1


;==== Increment a Word
#DEFINE INCW(val)				inc val
#DEFCONT 				\	bne $+4
#DEFCONT					\	inc val+1


;==== Decrement a Word
#DEFINE DECW(val)				lda val
#DEFCONT 				\	bne $+4
#DEFCONT 				\	dec val+1
#DEFCONT 				\	dec val


;====  Multiply a word by 2
#DEFINE MULTW2(mem)			asl mem
#DEFCONT					\	rol mem+1


;====  Add two mem regs together and save in 1st
#DEFINE ADDW(mema,memb)		clc
#DEFCONT					\	lda mema
#DEFCONT					\	adc memb
#DEFCONT					\	sta mema
#DEFCONT					\	lda mema+1
#DEFCONT					\	adc memb+1
#DEFCONT					\	sta mema+1


;====  Subtract two mem regs and save in 1st
#DEFINE SUBW(mema,memb)		sec
#DEFCONT					\	lda mema
#DEFCONT					\	sbc memb
#DEFCONT					\	sta mema
#DEFCONT					\	lda mema+1
#DEFCONT					\	sbc memb+1
#DEFCONT					\	sta mema+1

;=========================================
;====  Following used by Text Editor  ====
;=========================================

;====  Add Y reg to a ptr
#DEFINE ADDY(addr)			tya
#DEFCONT					\	clc
#DEFCONT					\	adc addr
#DEFCONT					\	sta addr
#DEFCONT					\	lda #$00
#DEFCONT					\	adc addr
#DEFCONT					\	sta addr


;====  Copy addr into ptr
#DEFINE CPYA2P(addr,ptr)		lda #<addr
#DEFCONT					\	sta ptr
#DEFCONT					\	lda #>addr
#DEFCONT					\	sta ptr+1


;==== Compare two address's
#DEFINE CMPP2W(addrst,addrcur)	lda #>addrst
#DEFCONT					\	cmp addrcur+1
#DEFCONT					\	bne $+6
#DEFCONT					\	lda #<addrst
#DEFCONT					\	cmp addrcur


;====  Compare from and until  ===
#DEFINE CMPFR2UN				lda ADFROM+1
#DEFCONT					\	cmp ADUNTIL+1
#DEFCONT					\	bne $+6
#DEFCONT					\	lda ADFROM
#DEFCONT					\	cmp ADUNTIL

;====  Compare start and ptr  ===
#DEFINE EMPTY_DOC?			lda #>TXTBEG
#DEFCONT					\	cmp TXTEND+1
#DEFCONT					\	bne $+6
#DEFCONT					\	lda #<TXTBEG
#DEFCONT					\	cmp TXTEND


;====  Compare start and ptr  ===
#DEFINE CPTR_AT_START?			lda #>TXTBEG
#DEFCONT					\	cmp TXTCURR+1
#DEFCONT					\	bne $+6
#DEFCONT					\	lda #<TXTBEG
#DEFCONT					\	cmp TXTCURR


;====  Compare start and ptr  ===
#DEFINE CPTR_AT_END?			lda TXTCURR+1
#DEFCONT					\	cmp TXTEND+1
#DEFCONT					\	bne $+6
#DEFCONT					\	lda TXTCURR
#DEFCONT					\	cmp TXTEND

;====  Is ptr == curr line?
#DEFINE ISCURRLINE?			lda TXTCURR+1
#DEFCONT					\	cmp CURRLINE+1
#DEFCONT					\	bne $+6
#DEFCONT					\	lda TXTCURR
#DEFCONT					\	cmp CURRLINE


;====  save curr ptr  ===
#DEFINE SAVE_PTRS				lda TXTCURR+1
#DEFCONT					\	pha
#DEFCONT					\	lda TXTCURR
#DEFCONT					\	pha
#DEFCONT					\	lda TXTEND+1
#DEFCONT					\	pha
#DEFCONT					\	lda TXTEND
#DEFCONT					\	pha
#DEFCONT					\	phy
#DEFCONT					\	phx
#DEFCONT					\	pha

;====  save curr ptr  ===
#DEFINE RESTORE_PTRS			pla
#DEFCONT					\	plx
#DEFCONT					\	ply
#DEFCONT					\	pla
#DEFCONT					\	sta TXTEND
#DEFCONT					\	pla
#DEFCONT					\	sta TXTEND+1
#DEFCONT					\	pla
#DEFCONT					\	sta TXTCURR
#DEFCONT					\	pla
#DEFCONT					\	sta TXTCURR+1



