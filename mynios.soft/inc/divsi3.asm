	.file	"divsi3.c"
	.section	.text
	.align	2
	.type	divmodsi4, @function
divmodsi4:
	movi	r3, 1
	mov	r7, zero
	br	.L2
.L3:
	add	r6, r6, r6
	add	r3, r3, r3
.L2:
	bltu	r6, r5, .L4
.L5:
	mov	r2, zero
	br	.L6
.L4:
	movi	r2, 32
	beq	r7, r2, .L5
	addi	r7, r7, 1
	bge	r6, zero, .L3
	br	.L5
.L8:
	bltu	r5, r6, .L9
	sub	r5, r5, r6
	or	r2, r2, r3
.L9:
	srai	r3, r3, 1
	srli	r6, r6, 1
.L6:
	bne	r3, zero, .L8
	bne	r4, zero, .L12
	mov	r5, r2
.L12:
	mov	r2, r5
	ret	
	.size	divmodsi4, .-divmodsi4
	.align	2
	.global	__modsi3
	.type	__modsi3, @function
__modsi3:
	mov	r2, r5
	mov	r3, r4
	addi	sp, sp, -4
	mov	r5, r4
	mov	r7, r2
	mov	r6, r2
	stw	ra, 0(sp)
	movi	r4, 1
	sub	r8, zero, r3
	cmpge	r2, r2, zero
	bge	r3, zero, .L16
	bne	r2, zero, .L18
	sub	r7, zero, r7
	br	.L18
.L16:
	bne	r2, zero, .L20
	sub	r6, zero, r7
.L20:
	ldw	ra, 0(sp)
	addi	sp, sp, 4
	jmpi	divmodsi4
.L18:
	mov	r5, r8
	mov	r6, r7
	movi	r4, 1
	call	divmodsi4
	sub	r2, zero, r2
	ldw	ra, 0(sp)
	addi	sp, sp, 4
	ret	
	.size	__modsi3, .-__modsi3
	.align	2
	.global	__divsi3
	.type	__divsi3, @function
__divsi3:
	addi	sp, sp, -8
	stw	r16, 0(sp)
	stw	ra, 4(sp)
	mov	r2, r4
	mov	r16, zero
	mov	r6, r5
	bge	r4, zero, .L25
	sub	r2, zero, r4
	movi	r16, 1
.L25:
	mov	r5, r2
	mov	r4, zero
	bge	r6, zero, .L26
	movi	r2, 1
	sub	r16, r2, r16
	sub	r6, zero, r6
.L26:
	call	divmodsi4
	beq	r16, zero, .L28
	sub	r2, zero, r2
.L28:
	ldw	ra, 4(sp)
	ldw	r16, 0(sp)
	addi	sp, sp, 8
	ret	
	.size	__divsi3, .-__divsi3
	.align	2
	.global	__umodsi3
	.type	__umodsi3, @function
__umodsi3:
	mov	r6, r5
	mov	r5, r4
	movi	r4, 1
	jmpi	divmodsi4
	.size	__umodsi3, .-__umodsi3
	.align	2
	.global	__udivsi3
	.type	__udivsi3, @function
__udivsi3:
	mov	r6, r5
	mov	r5, r4
	mov	r4, zero
	jmpi	divmodsi4
	.size	__udivsi3, .-__udivsi3
	.ident	"GCC: (Altera 13.0sp1 Build 232) 4.1.2"
