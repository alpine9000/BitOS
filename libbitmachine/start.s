.extern _data
.extern _mdata
.extern _ebss
.extern _bss	
.extern _main
	
.global vector
.global _reset
.global start
	.global _init_ctors
.section .text

_reset:
	! initialise sections
	mov.l	edata,r1		! edata in r1
	mov.l	mdata,r2		! mdata in r2
	mov.l	data,r0		        ! data in r0
	cmp/eq  r0,r1
	bt	rstart_1
	nop
rstart_l:
	mov.b   @r2,r3  !get from src
	mov.b   r3,@r0  !place in dest
	add   	#1,r2    !inc src
	add   	#1,r0    !inc dest
	cmp/eq  r0,r1   !dest == edata?
	bf	rstart_l
	nop
rstart_1:

	! zero out bss
	mov.l	ebss,r1
	mov.l	bss,r0
	cmp/eq	r0,r1         
	bt	rstart_3
	sub	r2,r2
rstart_2:
	mov.b	r2,@r0
	add	#1,r0
	cmp/eq	r0,r1
	bf     	rstart_2
	nop
rstart_3:
	mov.l   init_ctors,r1
	jsr     @r1
	nop
	mov.l 	__main,r1
	jsr     @r1
	nop



.align 4
init_ctors:
		.long _init_ctors
_start:
		.long start
__main:
		.long _main
data:
		.long	_data
mdata:
		.long	_mdata
edata:
		.long	_edata
bss:
		.long	_bss
ebss:
		.long	_ebss
	