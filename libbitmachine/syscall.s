.extern _syscall_table
.global __syscall
.global __panic
.global __illegalSlot


.align 4	
__syscall:
	sts.l pr,@-r15
	mov.l table,r0
	mov #4,r1
	shll2 r4
	add r4,r0
	mov r5,r4
	mov r6,r5
	mov r7,r6
	mov.l @r0,r0
	jsr @r0
	mov r8,r7	
	lds.l @r15+,pr
	rte
	nop
	
.align	2	
table:
	.long	_syscall_table
	
__panic:
	!mov r15,r5
	!mov #4,r1
	!sub r1,r15
	!mov #4,r1
	!mov.l @r15,r5
	mov.l panic,r0
	jsr @r0
	!sub r1,r5
	nop

.align 2
panic:
	.long _handlePanic

__illegalSlot:
	mov r15,r4
	mov #4,r1
	mov.l @r15,r4
	mov.l illegalSlot,r0
	jsr @r0
	sub r1,r4

.align 2
illegalSlot:
	.long _illegalSlot


	