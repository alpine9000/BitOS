	.file	"ucnid-6.c"
	.text
	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.align 1
	.global	_À
	.type	_À, @function
_À:
.LFB0:
	.file 1 "ucnid-6.c"
	.loc 1 7 0
	.cfi_startproc
	mov.l	r14,@-r15
	.cfi_def_cfa_offset 4
	.cfi_offset 14, -4
	mov	r15,r14
	.cfi_def_cfa_register 14
	.loc 1 7 0
	mov	#1,r1
	lds	r1,fpul
	fsts	fpul,fr1
	flds	fr1,fpul
	sts	fpul,r0
	mov	r14,r15
	.cfi_def_cfa_register 15
	mov.l	@r15+,r14
	.cfi_restore 14
	.cfi_def_cfa_offset 0
	rts	
	nop
	.cfi_endproc
.LFE0:
	.size	_À, .-_À
	.align 1
	.global	_Á
	.type	_Á, @function
_Á:
.LFB1:
	.loc 1 8 0
	.cfi_startproc
	mov.l	r14,@-r15
	.cfi_def_cfa_offset 4
	.cfi_offset 14, -4
	mov	r15,r14
	.cfi_def_cfa_register 14
	.loc 1 8 0
	mov	#2,r1
	lds	r1,fpul
	fsts	fpul,fr1
	flds	fr1,fpul
	sts	fpul,r0
	mov	r14,r15
	.cfi_def_cfa_register 15
	mov.l	@r15+,r14
	.cfi_restore 14
	.cfi_def_cfa_offset 0
	rts	
	nop
	.cfi_endproc
.LFE1:
	.size	_Á, .-_Á
	.align 1
	.global	_Â
	.type	_Â, @function
_Â:
.LFB2:
	.loc 1 9 0
	.cfi_startproc
	mov.l	r14,@-r15
	.cfi_def_cfa_offset 4
	.cfi_offset 14, -4
	mov	r15,r14
	.cfi_def_cfa_register 14
	.loc 1 9 0
	mov	#3,r1
	lds	r1,fpul
	fsts	fpul,fr1
	flds	fr1,fpul
	sts	fpul,r0
	mov	r14,r15
	.cfi_def_cfa_register 15
	mov.l	@r15+,r14
	.cfi_restore 14
	.cfi_def_cfa_offset 0
	rts	
	nop
	.cfi_endproc
.LFE2:
	.size	_Â, .-_Â
	.align 1
	.global	_whÿ
	.type	_whÿ, @function
_whÿ:
.LFB3:
	.loc 1 10 0
	.cfi_startproc
	mov.l	r14,@-r15
	.cfi_def_cfa_offset 4
	.cfi_offset 14, -4
	mov	r15,r14
	.cfi_def_cfa_register 14
	.loc 1 10 0
	mov	#4,r1
	lds	r1,fpul
	fsts	fpul,fr1
	flds	fr1,fpul
	sts	fpul,r0
	mov	r14,r15
	.cfi_def_cfa_register 15
	mov.l	@r15+,r14
	.cfi_restore 14
	.cfi_def_cfa_offset 0
	rts	
	nop
	.cfi_endproc
.LFE3:
	.size	_whÿ, .-_whÿ
	.align 1
	.global	_aÄbсδe
	.type	_aÄbсδe, @function
_aÄbсδe:
.LFB4:
	.loc 1 11 0
	.cfi_startproc
	mov.l	r14,@-r15
	.cfi_def_cfa_offset 4
	.cfi_offset 14, -4
	mov	r15,r14
	.cfi_def_cfa_register 14
	.loc 1 11 0
	mov	#5,r1
	lds	r1,fpul
	fsts	fpul,fr1
	flds	fr1,fpul
	sts	fpul,r0
	mov	r14,r15
	.cfi_def_cfa_register 15
	mov.l	@r15+,r14
	.cfi_restore 14
	.cfi_def_cfa_offset 0
	rts	
	nop
	.cfi_endproc
.LFE4:
	.size	_aÄbсδe, .-_aÄbсδe
	.align 1
	.global	_main
	.type	_main, @function
_main:
.LFB5:
	.loc 1 14 0
	.cfi_startproc
	mov.l	r14,@-r15
	.cfi_def_cfa_offset 4
	.cfi_offset 14, -4
	sts.l	pr,@-r15
	.cfi_def_cfa_offset 8
	.cfi_offset 17, -8
	mov	r15,r14
	.cfi_def_cfa_register 14
	.loc 1 16 0
	mov.l	.L18,r1
	jsr	@r1
	nop
	mov	r0,r2
	mov	#1,r1
	cmp/eq	r1,r2
	bt	.L12
	.loc 1 17 0
	mov.l	.L27,r1
	jsr	@r1
	nop
	.align 1
.L12:
	.loc 1 18 0
	mov.l	.L20,r1
	jsr	@r1
	nop
	mov	r0,r2
	mov	#2,r1
	cmp/eq	r1,r2
	bt	.L13
	.loc 1 19 0
	mov.l	.L27,r1
	jsr	@r1
	nop
	.align 1
.L13:
	.loc 1 20 0
	mov.l	.L22,r1
	jsr	@r1
	nop
	mov	r0,r2
	mov	#3,r1
	cmp/eq	r1,r2
	bt	.L14
	.loc 1 21 0
	mov.l	.L27,r1
	jsr	@r1
	nop
	.align 1
.L14:
	.loc 1 22 0
	mov.l	.L24,r1
	jsr	@r1
	nop
	mov	r0,r2
	mov	#4,r1
	cmp/eq	r1,r2
	bt	.L15
	.loc 1 23 0
	mov.l	.L27,r1
	jsr	@r1
	nop
	.align 1
.L15:
	.loc 1 24 0
	mov.l	.L26,r1
	jsr	@r1
	nop
	mov	r0,r2
	mov	#5,r1
	cmp/eq	r1,r2
	bt	.L16
	.loc 1 25 0
	mov.l	.L27,r1
	jsr	@r1
	nop
	.align 1
.L16:
	.loc 1 27 0
	mov	#0,r1
	lds	r1,fpul
	fsts	fpul,fr1
	.loc 1 28 0
	flds	fr1,fpul
	sts	fpul,r0
	mov	r14,r15
	.cfi_def_cfa_register 15
	lds.l	@r15+,pr
	.cfi_restore 17
	.cfi_def_cfa_offset 4
	mov.l	@r15+,r14
	.cfi_restore 14
	.cfi_def_cfa_offset 0
	rts	
	nop
.L28:
	.align 2
.L18:
	.long	_À
.L27:
	.long	_abort
.L20:
	.long	_Á
.L22:
	.long	_Â
.L24:
	.long	_whÿ
.L26:
	.long	_aÄbсδe
	.cfi_endproc
.LFE5:
	.size	_main, .-_main
.Letext0:
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.ualong	0xa4
	.uaword	0x4
	.ualong	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.ualong	.LASF3
	.byte	0xc
	.ualong	.LASF4
	.ualong	.LASF5
	.ualong	.Ltext0
	.ualong	.Letext0-.Ltext0
	.ualong	.Ldebug_line0
	.uleb128 0x2
	.string	"\303\200"
	.byte	0x1
	.byte	0x7
	.ualong	0x39
	.ualong	.LFB0
	.ualong	.LFE0-.LFB0
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x2
	.string	"\303\201"
	.byte	0x1
	.byte	0x8
	.ualong	0x39
	.ualong	.LFB1
	.ualong	.LFE1-.LFB1
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x2
	.string	"\303\202"
	.byte	0x1
	.byte	0x9
	.ualong	0x39
	.ualong	.LFB2
	.ualong	.LFE2-.LFB2
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x4
	.ualong	.LASF0
	.byte	0x1
	.byte	0xa
	.ualong	0x39
	.ualong	.LFB3
	.ualong	.LFE3-.LFB3
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x4
	.ualong	.LASF1
	.byte	0x1
	.byte	0xb
	.ualong	0x39
	.ualong	.LFB4
	.ualong	.LFE4-.LFB4
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x5
	.ualong	.LASF2
	.byte	0x1
	.byte	0xd
	.ualong	0x39
	.ualong	.LFB5
	.ualong	.LFE5-.LFB5
	.uleb128 0x1
	.byte	0x9c
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2116
	.uleb128 0x19
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",@progbits
	.ualong	0x1c
	.uaword	0x2
	.ualong	.Ldebug_info0
	.byte	0x4
	.byte	0
	.uaword	0
	.uaword	0
	.ualong	.Ltext0
	.ualong	.Letext0-.Ltext0
	.ualong	0
	.ualong	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF3:
	.string	"GNU C99 5.3.0 -m2e -g -std=c99"
.LASF1:
	.string	"a\303\204b\321\201\316\264e"
.LASF2:
	.string	"main"
.LASF4:
	.string	"ucnid-6.c"
.LASF0:
	.string	"wh\303\277"
.LASF5:
	.string	"/Users/alex/Projects/BitOS/tests/gcc.dg"
	.ident	"GCC: (GNU) 5.3.0"
