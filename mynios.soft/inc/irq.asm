irqPtr:
	.zero	4
irqCall:
	addi	sp, sp, -100
	stw	ra ,0(sp) 
	stw	r1 ,4(sp) 
	stw	r2 ,8(sp) 
	stw	r3 ,12(sp)
	stw	r4 ,16(sp)
	stw	r5 ,20(sp)
	stw	r6 ,24(sp)
	stw	r7 ,28(sp)
	stw	r8 ,32(sp)
	stw	r9 ,36(sp)
	stw	r10,40(sp)
	stw	r11,44(sp)
	stw	r12,48(sp)
	stw	r13,52(sp)
	stw	r14,56(sp)
	stw	r15,60(sp)
	stw	r16,64(sp)
	stw	r17,68(sp)
	stw	r18,72(sp)
	stw	r19,76(sp)
	stw	r20,80(sp)
	stw	r21,84(sp)
	stw	r22,88(sp)
	stw	r23,92(sp)
	stw	fp ,96(sp)

	ldw	r2, %gprel(irqPtr)(gp)
	callr	r2

	ldw	ra ,0(sp) 
	ldw	r1 ,4(sp) 
	ldw	r2 ,8(sp) 
	ldw	r3 ,12(sp)
	ldw	r4 ,16(sp)
	ldw	r5 ,20(sp)
	ldw	r6 ,24(sp)
	ldw	r7 ,28(sp)
	ldw	r8 ,32(sp)
	ldw	r9 ,36(sp)
	ldw	r10,40(sp)
	ldw	r11,44(sp)
	ldw	r12,48(sp)
	ldw	r13,52(sp)
	ldw	r14,56(sp)
	ldw	r15,60(sp)
	ldw	r16,64(sp)
	ldw	r17,68(sp)
	ldw	r18,72(sp)
	ldw	r19,76(sp)
	ldw	r20,80(sp)
	ldw	r21,84(sp)
	ldw	r22,88(sp)
	ldw	r23,92(sp)
	ldw	fp ,96(sp)

	addi	sp, sp, 100
	reti

setIrq:
  stw r4, %gprel(irqPtr)(gp)
  movhi r2, %hiadj(irqCall)
  addi r2, r2, %lo(irqCall)
  setirq r2, r5, r0, 1
exit_setIrq:
  ret

sti:
  setirq r0, r4, r2, 0
  ret
