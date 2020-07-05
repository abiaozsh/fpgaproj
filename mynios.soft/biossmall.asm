	.file	"bios.cpp"
	.section	.text
	.align	2
	.global	_Z10combineIntcccc
	.type	_Z10combineIntcccc, @function
_Z10combineIntcccc:
	slli	r6,r6,16
	or	r2, r4, r6
	ret	
	.size	_Z10combineIntcccc, .-_Z10combineIntcccc
	.align	2
	.global	_Z20SPI_CHIP_SELECT_HIGHv
	.type	_Z20SPI_CHIP_SELECT_HIGHv, @function
_Z20SPI_CHIP_SELECT_HIGHv:
	movi	r3, 7
	movhi	r2, %hiadj(33751052)
	addi	r2, r2, %lo(33751052)
	stwio	r3, 0(r2)
	ret	
	.size	_Z20SPI_CHIP_SELECT_HIGHv, .-_Z20SPI_CHIP_SELECT_HIGHv
	.section	.text._Z4_dlyv,"axG",@progbits,_Z4_dlyv,comdat
	.align	2
	.weak	_Z4_dlyv
	.type	_Z4_dlyv, @function
_Z4_dlyv:
	movhi	r2, 513
	stwio	zero, 0(r2)
	mov	r4, r2
	movi	r3, 20
.L6:
	ldwio	r2, 0(r4)
	bge	r3, r2, .L6
	ret	
	.size	_Z4_dlyv, .-_Z4_dlyv
	.section	.text
	.align	2
	.global	_Z7spiSendi
	.type	_Z7spiSendi, @function
_Z7spiSendi:
	addi	sp, sp, -16
	stw	r17, 4(sp)
	stw	r16, 0(sp)
	stw	ra, 12(sp)
	stw	r18, 8(sp)
	mov	r16, r4
	mov	r17, zero
.L12:
	movhi	r2, %hiadj(33751048)
	addi	r2, r2, %lo(33751048)
	stwio	zero, 0(r2)
	call	_Z4_dlyv
	andi	r2, r16, 128
	beq	r2, zero, .L13
	movi	r3, 1
	movhi	r2, %hiadj(33751044)
	addi	r2, r2, %lo(33751044)
	stwio	r3, 0(r2)
	br	.L15
.L13:
	movhi	r2, %hiadj(33751044)
	addi	r2, r2, %lo(33751044)
	stwio	zero, 0(r2)
.L15:
	movhi	r18, %hiadj(33751048)
	addi	r18, r18, %lo(33751048)
	movi	r2, 1
	stwio	r2, 0(r18)
	add	r17, r17, r2
	call	_Z4_dlyv
	movi	r2, 8
	add	r16, r16, r16
	bne	r17, r2, .L12
	stwio	zero, 0(r18)
	ldw	ra, 12(sp)
	ldw	r18, 8(sp)
	ldw	r17, 4(sp)
	ldw	r16, 0(sp)
	addi	sp, sp, 16
	ret	
	.size	_Z7spiSendi, .-_Z7spiSendi
	.align	2
	.global	_Z6spiRecv
	.type	_Z6spiRecv, @function
_Z6spiRecv:
	addi	sp, sp, -16
	movi	r3, 1
	movhi	r2, %hiadj(33751044)
	addi	r2, r2, %lo(33751044)
	stw	ra, 12(sp)
	stw	r18, 8(sp)
	stw	r17, 4(sp)
	stw	r16, 0(sp)
	stwio	r3, 0(r2)
	mov	r16, zero
	mov	r17, zero
.L20:
	movhi	r18, %hiadj(33751048)
	addi	r18, r18, %lo(33751048)
	movi	r2, 1
	stwio	r2, 0(r18)
	call	_Z4_dlyv
	add	r16, r16, r16
	movhi	r2, 515
	ldwio	r2, 0(r2)
	beq	r2, zero, .L21
	ori	r16, r16, 1
.L21:
	stwio	zero, 0(r18)
	call	_Z4_dlyv
	addi	r17, r17, 1
	movi	r2, 8
	bne	r17, r2, .L20
	mov	r2, r16
	ldw	ra, 12(sp)
	ldw	r18, 8(sp)
	ldw	r17, 4(sp)
	ldw	r16, 0(sp)
	addi	sp, sp, 16
	ret	
	.size	_Z6spiRecv, .-_Z6spiRecv
	.align	2
	.global	_Z22Sd2Card_waitStartBlockP6SDcard
	.type	_Z22Sd2Card_waitStartBlockP6SDcard, @function
_Z22Sd2Card_waitStartBlockP6SDcard:
	addi	sp, sp, -8
	stw	r16, 0(sp)
	stw	ra, 4(sp)
	mov	r16, zero
.L28:
	call	_Z6spiRecv
	mov	r3, r2
	movi	r2, 255
	addi	r16, r16, 1
	bne	r3, r2, .L29
	movhi	r2, %hiadj(300000)
	addi	r2, r2, %lo(300000)
	bne	r16, r2, .L28
	mov	r2, zero
	br	.L32
.L29:
	cmpeqi	r2, r3, 254
.L32:
	ldw	ra, 4(sp)
	ldw	r16, 0(sp)
	addi	sp, sp, 8
	ret	
	.size	_Z22Sd2Card_waitStartBlockP6SDcard, .-_Z22Sd2Card_waitStartBlockP6SDcard
	.align	2
	.global	_Z19Sd2Card_waitNotBusyi
	.type	_Z19Sd2Card_waitNotBusyi, @function
_Z19Sd2Card_waitNotBusyi:
	addi	sp, sp, -8
	stw	r16, 0(sp)
	movhi	r2, 513
	mov	r16, r4
	stw	ra, 4(sp)
	stwio	zero, 0(r2)
.L37:
	call	_Z6spiRecv
	movi	r3, 255
	bne	r2, r3, .L38
	movi	r2, 1
	br	.L40
.L38:
	movhi	r2, 513
	ldwio	r2, 0(r2)
	bge	r16, r2, .L37
	mov	r2, zero
.L40:
	ldw	ra, 4(sp)
	ldw	r16, 0(sp)
	addi	sp, sp, 8
	ret	
	.size	_Z19Sd2Card_waitNotBusyi, .-_Z19Sd2Card_waitNotBusyi
	.align	2
	.global	_Z19Sd2Card_cardCommandP6SDcardii
	.type	_Z19Sd2Card_cardCommandP6SDcardii, @function
_Z19Sd2Card_cardCommandP6SDcardii:
	ldw	r2, 512(r4)
	movi	r3, -2
	addi	sp, sp, -16
	rol	r3, r3, r2
	stw	r18, 8(sp)
	stw	r17, 4(sp)
	mov	r18, r6
	mov	r17, r5
	movhi	r2, %hiadj(33751052)
	addi	r2, r2, %lo(33751052)
	stw	ra, 12(sp)
	stw	r16, 0(sp)
	stwio	r3, 0(r2)
	movhi	r4, %hiadj(3000000)
	addi	r4, r4, %lo(3000000)
	call	_Z19Sd2Card_waitNotBusyi
	ori	r4, r17, 64
	call	_Z7spiSendi
	movi	r16, 24
.L46:
	sra	r4, r18, r16
	addi	r16, r16, -8
	call	_Z7spiSendi
	movi	r2, -8
	bne	r16, r2, .L46
	bne	r17, zero, .L48
	movi	r4, 149
	br	.L50
.L48:
	movi	r4, 255
.L50:
	call	_Z7spiSendi
	mov	r16, zero
.L51:
	call	_Z6spiRecv
	mov	r3, r2
	andi	r2, r2, 128
	beq	r2, zero, .L52
	movi	r2, 255
	beq	r16, r2, .L52
	addi	r16, r16, 1
	br	.L51
.L52:
	mov	r2, r3
	ldw	ra, 12(sp)
	ldw	r18, 8(sp)
	ldw	r17, 4(sp)
	ldw	r16, 0(sp)
	addi	sp, sp, 16
	ret	
	.size	_Z19Sd2Card_cardCommandP6SDcardii, .-_Z19Sd2Card_cardCommandP6SDcardii
	.align	2
	.global	_Z16Sd2Card_readDataP6SDcardi
	.type	_Z16Sd2Card_readDataP6SDcardi, @function
_Z16Sd2Card_readDataP6SDcardi:
	slli	r6,r5,9
	addi	sp, sp, -12
	movi	r5, 17
	stw	r17, 4(sp)
	stw	ra, 8(sp)
	stw	r16, 0(sp)
	mov	r17, r4
	call	_Z19Sd2Card_cardCommandP6SDcardii
	bne	r2, zero, .L59
	mov	r4, r17
	call	_Z22Sd2Card_waitStartBlockP6SDcard
	beq	r2, zero, .L59
	mov	r16, zero
.L62:
	call	_Z6spiRecv
	add	r3, r17, r16
	stb	r2, 0(r3)
	addi	r16, r16, 1
	movi	r2, 512
	bne	r16, r2, .L62
	call	_Z6spiRecv
	call	_Z6spiRecv
	movi	r2, 1
	br	.L64
.L59:
	mov	r2, zero
.L64:
	ldw	ra, 8(sp)
	ldw	r17, 4(sp)
	ldw	r16, 0(sp)
	addi	sp, sp, 12
	ret	
	.size	_Z16Sd2Card_readDataP6SDcardi, .-_Z16Sd2Card_readDataP6SDcardi
	.align	2
	.global	_Z16MMCCard_cardinitP6SDcard
	.type	_Z16MMCCard_cardinitP6SDcard, @function
_Z16MMCCard_cardinitP6SDcard:
	addi	sp, sp, -12
	stw	r17, 4(sp)
	stw	r16, 0(sp)
	stw	ra, 8(sp)
	mov	r17, r4
	mov	r16, zero
.L69:
	movi	r4, 255
	call	_Z7spiSendi
	addi	r16, r16, 1
	movi	r2, 20
	bne	r16, r2, .L69
	mov	r16, zero
.L71:
	mov	r4, r17
	mov	r5, zero
	mov	r6, zero
	call	_Z19Sd2Card_cardCommandP6SDcardii
	movi	r3, 1
	addi	r16, r16, 1
	beq	r2, r3, .L72
	movi	r2, 10
	beq	r16, r2, .L74
	br	.L71
.L80:
	movi	r2, 1
	br	.L78
.L72:
	mov	r16, zero
.L75:
	mov	r4, r17
	movi	r5, 1
	mov	r6, zero
	addi	r16, r16, 1
	call	_Z19Sd2Card_cardCommandP6SDcardii
	beq	r2, zero, .L80
	movi	r2, 100
	bne	r16, r2, .L75
.L74:
	mov	r2, zero
.L78:
	ldw	ra, 8(sp)
	ldw	r17, 4(sp)
	ldw	r16, 0(sp)
	addi	sp, sp, 12
	ret	
	.size	_Z16MMCCard_cardinitP6SDcard, .-_Z16MMCCard_cardinitP6SDcard
	.align	2
	.global	_Z10uart_writei
	.type	_Z10uart_writei, @function
_Z10uart_writei:
	movhi	r3, %hiadj(33685508)
	addi	r3, r3, %lo(33685508)
.L87:
	ldwio	r2, 0(r3)
	andi	r2, r2, 256
	bne	r2, zero, .L87
	stwio	r4, 0(r3)
	ret	
	.size	_Z10uart_writei, .-_Z10uart_writei
	.align	2
	.global	_Z5printPKc
	.type	_Z5printPKc, @function
_Z5printPKc:
	addi	sp, sp, -8
	stw	r16, 0(sp)
	stw	ra, 4(sp)
	mov	r16, r4
.L93:
	ldb	r2, 0(r16)
	addi	r16, r16, 1
	mov	r4, r2
	beq	r2, zero, .L97
	call	_Z10uart_writei
	br	.L93
.L97:
	ldw	ra, 4(sp)
	ldw	r16, 0(sp)
	addi	sp, sp, 8
	ret	
	.size	_Z5printPKc, .-_Z5printPKc
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
.LC0:
	.string	"INIT\r\n\0SD ok\r\n\0sign ok\r\n"
	.section	.text
	.align	2
	.global	main
	.type	main, @function
main:
	addi	sp, sp, -16
	movhi	r1, %hiadj(.LC0)
	addi	r1, r1, %lo(.LC0)
  addi r4, r1, 0
	call	_Z5printPKc
	movhi	r16, 16
	movi	r2, 0
	mov	r4, r16
	stw	r2, 512(r16)
	call	_Z16MMCCard_cardinitP6SDcard
	beq	r2, zero, .L99
	addi r4, r1, 7
	call	_Z5printPKc
	mov	r5, zero
	mov	r4, r16
	call	_Z16Sd2Card_readDataP6SDcardi
	ldhu	r4, 454(r16)
	ldhu	r6, 456(r16)
	call	_Z10combineIntcccc
	mov	r5, r2
	mov	r4, r16
	mov	r18, r2
	call	_Z16Sd2Card_readDataP6SDcardi
	ldhu	r4, 90(r16)
	ldhu	r6, 92(r16)
	call	_Z10combineIntcccc
	movhi	r3, %hiadj(2018915346)
	addi	r3, r3, %lo(2018915346)
	bne	r2, r3, .L99
	addi r4, r1, 15
	call	_Z5printPKc
	ldhu	r4, 94(r16)
	ldhu	r6, 96(r16)
	mov	r17, zero
	call	_Z10combineIntcccc
	add	r18, r2, r18
	ldhu	r4, 98(r16)
	ldhu	r6, 100(r16)
	call	_Z10combineIntcccc
	mov	r16, r2
	br	.L103
.L104:
	movhi	r4, 16
	add	r5, r18, r17
	call	_Z16Sd2Card_readDataP6SDcardi
	slli	r5,r17,9
	mov	r4, zero
	movhi	r7, 16
	movi	r6, 512
.L105:
	add	r2, r4, r7
	ldbu	r3, 0(r2)
	add	r2, r5, r4
	addi	r4, r4, 1
	stb	r3, 0(r2)
	bne	r4, r6, .L105
	addi	r17, r17, 1
.L103:
	blt	r17, r16, .L104
	call	_Z20SPI_CHIP_SELECT_HIGHv
#APP
	jmpi 0
#NO_APP
.L99:
	call	_Z20SPI_CHIP_SELECT_HIGHv
.L109:
	br	.L109
	.size	main, .-main
	.ident	"GCC: (Altera 13.0sp1 Build 232) 4.1.2"
