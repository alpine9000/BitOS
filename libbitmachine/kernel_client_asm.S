.global __kernel_atomic_lock_asm

__kernel_atomic_lock_asm:
	tas.b @r4
	movt r0
	rts
	nop
