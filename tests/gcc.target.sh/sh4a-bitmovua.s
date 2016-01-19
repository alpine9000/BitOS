	.file	"sh4a-bitmovua.c"
	.text
	.text
	.align 1
	.global	_f0
	.type	_f0, @function
_f0:
	mov.l	.L2,r1
	mov.l	@r1,r1
	mov	r1,r0
	shll	r0
	rts	
	subc	r0,r0
.L3:
	.align 2
.L2:
	.long	_x0
	.size	_f0, .-_f0
	.align 1
	.global	_f1
	.type	_f1, @function
_f1:
	mov.l	.L5,r3
	mov.l	@r3,r1
	mov	r1,r2
	shll8	r2
	mov.l	@(4,r3),r1
	shlr16	r1
	shlr8	r1
	or	r2,r1
	mov	r1,r0
	shll	r0
	rts	
	subc	r0,r0
.L6:
	.align 2
.L5:
	.long	_x1
	.size	_f1, .-_f1
	.align 1
	.global	_f2
	.type	_f2, @function
_f2:
	mov.l	.L8,r1
	mov.l	@r1,r2
	mov.l	@(4,r1),r1
	xtrct	r2,r1
	mov	r1,r0
	shll	r0
	rts	
	subc	r0,r0
.L9:
	.align 2
.L8:
	.long	_x2
	.size	_f2, .-_f2
	.align 1
	.global	_f3
	.type	_f3, @function
_f3:
	mov.l	.L11,r3
	mov.l	@r3,r1
	mov	r1,r2
	shll16	r2
	shll8	r2
	mov.l	@(4,r3),r1
	shlr8	r1
	or	r2,r1
	mov	r1,r0
	shll	r0
	rts	
	subc	r0,r0
.L12:
	.align 2
.L11:
	.long	_x3
	.size	_f3, .-_f3
	.align 1
	.global	_f4
	.type	_f4, @function
_f4:
	mov.l	.L14,r1
	mov.l	@(4,r1),r1
	mov	r1,r0
	shll	r0
	rts	
	subc	r0,r0
.L15:
	.align 2
.L14:
	.long	_x4
	.size	_f4, .-_f4
	.align 1
	.global	_g0
	.type	_g0, @function
_g0:
	mov.l	.L17,r1
	mov.l	@r1,r1
	rts	
	mov	#0,r0
.L18:
	.align 2
.L17:
	.long	_y_0
	.size	_g0, .-_g0
	.align 1
	.global	_g1
	.type	_g1, @function
_g1:
	mov.l	.L20,r1
	mov.l	@r1,r2
	shll8	r2
	mov.l	@(4,r1),r1
	shlr16	r1
	shlr8	r1
	or	r2,r1
	rts	
	mov	#0,r0
.L21:
	.align 2
.L20:
	.long	_y_1
	.size	_g1, .-_g1
	.align 1
	.global	_g2
	.type	_g2, @function
_g2:
	mov.l	.L23,r1
	mov.l	@r1,r2
	mov.l	@(4,r1),r1
	xtrct	r2,r1
	rts	
	mov	#0,r0
.L24:
	.align 2
.L23:
	.long	_y2
	.size	_g2, .-_g2
	.align 1
	.global	_g3
	.type	_g3, @function
_g3:
	mov.l	.L26,r1
	mov.l	@r1,r2
	shll16	r2
	shll8	r2
	mov.l	@(4,r1),r1
	shlr8	r1
	or	r2,r1
	rts	
	mov	#0,r0
.L27:
	.align 2
.L26:
	.long	_y3
	.size	_g3, .-_g3
	.align 1
	.global	_g4
	.type	_g4, @function
_g4:
	mov.l	.L29,r1
	mov.l	@(4,r1),r1
	rts	
	mov	#0,r0
.L30:
	.align 2
.L29:
	.long	_y4
	.size	_g4, .-_g4
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align 2
.LC0:
	.string	"f1 () == 0x12345678"
	.align 2
.LC1:
	.string	"sh4a-bitmovua.c"
	.align 2
.LC2:
	.string	"f2 () == 0x12345678"
	.align 2
.LC3:
	.string	"f3 () == 0x12345678"
	.align 2
.LC4:
	.string	"g1 () == 0x12345678"
	.align 2
.LC5:
	.string	"g2 () == 0x12345678"
	.align 2
.LC6:
	.string	"g3 () == 0x12345678"
	.text
	.align 1
	.global	_main
	.type	_main, @function
_main:
	sts.l	pr,@-r15
	mov.l	.L51,r1
	mov.l	@r1,r3
	mov.l	.L82,r2
	and	r3,r2
	mov.l	.L83,r3
	or	r3,r2
	mov.l	r2,@r1
	mov.l	@(4,r1),r3
	mov.l	.L84,r2
	and	r3,r2
	mov.l	.L85,r3
	or	r3,r2
	mov.l	.L56,r0
	jsr	@r0
	mov.l	r2,@(4,r1)
	tst	r0,r0
	bf	.L44
	mov.l	.L106,r2
	cmp/eq	r2,r1
	bt	.L32
.L44:
	mov.l	.L58,r7
	mov.l	.L108,r6
	mov.l	.L109,r4
	mov.l	.L110,r1
	jsr	@r1
	mov	#73,r5
	.align 1
.L32:
	mov.l	.L62,r1
	mov.l	@r1,r3
	mov.l	.L93,r2
	and	r3,r2
	mov.w	.L94,r3
	or	r3,r2
	mov.l	r2,@r1
	mov.l	@(4,r1),r2
	extu.w	r2,r3
	mov.l	.L95,r2
	or	r3,r2
	mov.l	.L66,r0
	jsr	@r0
	mov.l	r2,@(4,r1)
	tst	r0,r0
	bf	.L45
	mov.l	.L106,r2
	cmp/eq	r2,r1
	bt	.L34
.L45:
	mov.l	.L68,r7
	mov.l	.L108,r6
	mov.l	.L109,r4
	mov.l	.L110,r1
	jsr	@r1
	mov	#76,r5
	.align 1
.L34:
	mov.l	.L72,r2
	mov.l	@r2,r0
	mov.w	.L103,r1
	and	r1,r0
	or	#18,r0
	mov.l	r0,@r2
	mov.l	@(4,r2),r1
	extu.b	r1,r3
	mov.l	.L104,r1
	or	r3,r1
	mov.l	.L75,r0
	jsr	@r0
	mov.l	r1,@(4,r2)
	tst	r0,r0
	bf	.L46
	mov.l	.L106,r2
	cmp/eq	r2,r1
	bt	.L36
.L46:
	mov.l	.L77,r7
	mov.l	.L108,r6
	mov.l	.L109,r4
	mov.l	.L110,r1
	jsr	@r1
	mov	#79,r5
	.align 1
.L36:
	mov.l	.L81,r1
	mov.l	@r1,r3
	mov.l	.L82,r2
	and	r3,r2
	mov.l	.L83,r3
	or	r3,r2
	mov.l	r2,@r1
	mov.l	@(4,r1),r3
	mov.l	.L84,r2
	and	r3,r2
	mov.l	.L85,r3
	or	r3,r2
	mov.l	.L86,r0
	jsr	@r0
	mov.l	r2,@(4,r1)
	tst	r0,r0
	bf	.L47
	mov.l	.L106,r2
	cmp/eq	r2,r1
	bt	.L38
.L47:
	mov.l	.L88,r7
	mov.l	.L108,r6
	mov.l	.L109,r4
	mov.l	.L110,r1
	jsr	@r1
	mov	#82,r5
	.align 1
.L38:
	mov.l	.L92,r1
	mov.l	@r1,r3
	mov.l	.L93,r2
	and	r3,r2
	mov.w	.L94,r3
	or	r3,r2
	mov.l	r2,@r1
	mov.l	@(4,r1),r2
	extu.w	r2,r3
	mov.l	.L95,r2
	or	r3,r2
	mov.l	.L96,r0
	jsr	@r0
	mov.l	r2,@(4,r1)
	tst	r0,r0
	bf	.L48
	mov.l	.L106,r2
	cmp/eq	r2,r1
	bt	.L40
.L48:
	mov.l	.L98,r7
	mov.l	.L108,r6
	mov.l	.L109,r4
	mov.l	.L110,r1
	jsr	@r1
	mov	#85,r5
	.align 1
.L40:
	mov.l	.L102,r2
	mov.l	@r2,r0
	mov.w	.L103,r1
	and	r1,r0
	or	#18,r0
	mov.l	r0,@r2
	mov.l	@(4,r2),r1
	extu.b	r1,r3
	mov.l	.L104,r1
	or	r3,r1
	mov.l	.L105,r0
	jsr	@r0
	mov.l	r1,@(4,r2)
	tst	r0,r0
	bf	.L49
	mov.l	.L106,r2
	cmp/eq	r2,r1
	bt	.L42
.L49:
	mov.l	.L107,r7
	mov.l	.L108,r6
	mov.l	.L109,r4
	mov.l	.L110,r1
	jsr	@r1
	mov	#88,r5
	.align 1
.L42:
	mov	#0,r0
	lds.l	@r15+,pr
	rts	
	nop
	.align 1
.L94:
	.short	4660
.L103:
	.short	-256
.L111:
	.align 2
.L51:
	.long	_x1
.L82:
	.long	-16777216
.L83:
	.long	1193046
.L84:
	.long	16777215
.L85:
	.long	2013265920
.L56:
	.long	_f1
.L106:
	.long	305419896
.L58:
	.long	.LC0
.L108:
	.long	___func__.1533
.L109:
	.long	.LC1
.L110:
	.long	___assert_func
.L62:
	.long	_x2
.L93:
	.long	-65536
.L95:
	.long	1450704896
.L66:
	.long	_f2
.L68:
	.long	.LC2
.L72:
	.long	_x3
.L104:
	.long	878082048
.L75:
	.long	_f3
.L77:
	.long	.LC3
.L81:
	.long	_y_1
.L86:
	.long	_g1
.L88:
	.long	.LC4
.L92:
	.long	_y2
.L96:
	.long	_g2
.L98:
	.long	.LC5
.L102:
	.long	_y3
.L105:
	.long	_g3
.L107:
	.long	.LC6
	.size	_main, .-_main
	.section	.rodata
	.align 2
	.type	___func__.1533, @object
	.size	___func__.1533, 5
___func__.1533:
	.string	"main"
	.comm	_y4,8,4
	.comm	_y3,8,4
	.comm	_y2,8,4
	.comm	_y_1,8,4
	.comm	_y_0,4,4
	.comm	_x4,8,4
	.comm	_x3,8,4
	.comm	_x2,8,4
	.comm	_x1,8,4
	.comm	_x0,4,4
	.ident	"GCC: (GNU) 5.3.0"
