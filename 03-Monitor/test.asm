;==== Increment a Word
#DEFINE INCW(val)				inc val
#DEFCONT 				\	bne $+4
#DEFCONT					\	inc val+1


;==== Decrement a Word
#DEFINE DECW(val)				lda val
#DEFCONT 				\	bne $+4
#DEFCONT 				\	dec val+1
#DEFCONT 				\	dec val

#DEFINE CMPW(addrst,addrcur)	lda addrst+1
#DEFCONT					\	cmp addrcur+1
#DEFCONT					\	bne $+5
#DEFCONT					\	lda addrst
#DEFCONT					\	cmp addrcur


		*= $1000
		inc $05
		bne l1
		inc $06
l1
		lda $05
		bne l2
		dec $06
		dec $05
l2
		lda $06
		cmp $11
		bne l3
		lda $05
		cmp $10
l3
		nop

		
		lda $05+1
		cmp $10+1
		bne l4
		lda $05
		cmp $10
l4
		nop
		nop
		.end
		