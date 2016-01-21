.global __kernel_tick_asm
.global __kernel_blocked_asm
.global __kernel_kill_asm
.global __kernel_kill_thread_asm
.global __kernel_resume_asm
.global _stack
	
__kernel_resume_asm:
	mov r4,r15
	fmov.s @r15+,fr15
	fmov.s @r15+,fr14
	fmov.s @r15+,fr13
	fmov.s @r15+,fr12
	fmov.s @r15+,fr11
	fmov.s @r15+,fr10
	fmov.s @r15+,fr9
	fmov.s @r15+,fr8
	fmov.s @r15+,fr7
	fmov.s @r15+,fr6
	fmov.s @r15+,fr5
	fmov.s @r15+,fr4
	fmov.s @r15+,fr3
	fmov.s @r15+,fr2
	fmov.s @r15+,fr1
	fmov.s @r15+,fr0
	lds.l @r15+,fpul
	lds.l @r15+,fpscr
	mov.l @r15+,r14
	mov.l @r15+,r13
	mov.l @r15+,r12
	mov.l @r15+,r11
	mov.l @r15+,r10
	mov.l @r15+,r9
	mov.l @r15+,r8
	mov.l @r15+,r7
	mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
	mov.l @r15+,r2
	mov.l @r15+,r1
	mov.l @r15+,r0
	lds.l @r15+,macl
	lds.l @r15+,mach
	lds.l @r15+,pr
	rte
	nop

.align 4
__kernel_tick_asm:
    sts.l pr,@-r15	
    sts.l mach,@-r15
    sts.l macl,@-r15
    mov.l r0,@-r15
    mov.l r1,@-r15
    mov.l r2,@-r15
    mov.l r3,@-r15
    mov.l r4,@-r15
    mov.l r5,@-r15
    mov.l r6,@-r15
    mov.l r7,@-r15
    mov.l r8,@-r15
    mov.l r9,@-r15
    mov.l r10,@-r15
    mov.l r11,@-r15
    mov.l r12,@-r15
    mov.l r13,@-r15
    mov.l r14,@-r15
    sts.l fpscr,@-r15
    sts.l fpul,@-r15
    fmov.s fr0,@-r15
    fmov.s fr1,@-r15
    fmov.s fr2,@-r15
    fmov.s fr3,@-r15
    fmov.s fr4,@-r15
    fmov.s fr5,@-r15
    fmov.s fr6,@-r15
    fmov.s fr7,@-r15
    fmov.s fr8,@-r15
    fmov.s fr9,@-r15
    fmov.s fr10,@-r15
    fmov.s fr11,@-r15
    fmov.s fr12,@-r15
    fmov.s fr13,@-r15
    fmov.s fr14,@-r15
    fmov.s fr15,@-r15
    mov r15, r4
    mov.l kernel_stack,r0
    mov.l @r0,r15
    mov.l kernel_tick,r0
    jmp @r0
    nop
    rte
    nop

kernel_tick:
   .long __from_asm_kernel_tick

.align 4
__kernel_blocked_asm:
    sts.l pr,@-r15	
    sts.l mach,@-r15
    sts.l macl,@-r15
    mov.l r0,@-r15
    mov   #0xF,r0		! Disable interrupts when we don't disable them with the trap instruction - SR is already saved by TRAPA
    shll2 r0			
    shll2 r0			
    ldc r0,sr
    mov.l r1,@-r15
    mov.l r2,@-r15
    mov.l r3,@-r15
    mov.l r4,@-r15
    mov.l r5,@-r15
    mov.l r6,@-r15
    mov.l r7,@-r15
    mov.l r8,@-r15
    mov.l r9,@-r15
    mov.l r10,@-r15
    mov.l r11,@-r15
    mov.l r12,@-r15
    mov.l r13,@-r15
    mov.l r14,@-r15
    sts.l fpscr,@-r15
    sts.l fpul,@-r15
    fmov.s fr0,@-r15
    fmov.s fr1,@-r15
    fmov.s fr2,@-r15
    fmov.s fr3,@-r15
    fmov.s fr4,@-r15
    fmov.s fr5,@-r15
    fmov.s fr6,@-r15
    fmov.s fr7,@-r15
    fmov.s fr8,@-r15
    fmov.s fr9,@-r15
    fmov.s fr10,@-r15
    fmov.s fr11,@-r15
    fmov.s fr12,@-r15
    fmov.s fr13,@-r15
    fmov.s fr14,@-r15
    fmov.s fr15,@-r15
    mov r15, r4
    mov.l kernel_stack,r0
    mov.l @r0,r15
    mov.l kernel_blocked,r0
    jmp @r0
    nop
    rte
    nop

.align 4
__kernel_kill_thread_asm:
    sts.l pr,@-r15	
    sts.l mach,@-r15
    sts.l macl,@-r15
    mov.l r0,@-r15
    mov   #0xF,r0		! Disable interrupts when we don't disable them with the trap instruction - SR is already saved by TRAPA
    shll2 r0			
    shll2 r0			
    ldc r0,sr
    mov.l r1,@-r15
    mov.l r2,@-r15
    mov.l r3,@-r15
    mov.l r4,@-r15
    mov.l r5,@-r15
    mov.l r6,@-r15
    mov.l r7,@-r15
    mov.l r8,@-r15
    mov.l r9,@-r15
    mov.l r10,@-r15
    mov.l r11,@-r15
    mov.l r12,@-r15
    mov.l r13,@-r15
    mov.l r14,@-r15
    sts.l fpscr,@-r15
    sts.l fpul,@-r15
    fmov.s fr0,@-r15
    fmov.s fr1,@-r15
    fmov.s fr2,@-r15
    fmov.s fr3,@-r15
    fmov.s fr4,@-r15
    fmov.s fr5,@-r15
    fmov.s fr6,@-r15
    fmov.s fr7,@-r15
    fmov.s fr8,@-r15
    fmov.s fr9,@-r15
    fmov.s fr10,@-r15
    fmov.s fr11,@-r15
    fmov.s fr12,@-r15
    fmov.s fr13,@-r15
    fmov.s fr14,@-r15
    fmov.s fr15,@-r15
    mov r15, r4
    mov.l kernel_stack,r0
    mov.l @r0,r15
    mov.l kernel_kill_thread,r0
    jmp @r0
    nop
    rte
    nop

kernel_blocked:
   .long __from_asm_kernel_blocked

kernel_stack:
    .long _stack
	
.align 4
__kernel_kill_asm:
    mov  #0xF,r0	! Disable interrupts when we don't disable them with the trap instruction - SR is already saved by TRAPA
    shll2 r0			
    shll2 r0			
    ldc r0,sr
    mov.l kernel_kill,r0
    jsr @r0
    nop
    rte
    nop

.align 4
kernel_kill:
   .long __from_asm_kernel_kill


	.align 4
kernel_kill_thread:
   .long __from_asm_kernel_kill_thread


