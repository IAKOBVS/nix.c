	.file	"nix.c"
	.text
	.p2align 4
	.globl	nixRev
	.type	nixRev, @function
nixRev:
.LFB24:
	.cfi_startproc
	leaq	-1(%rsi,%rdx), %r9
	cmpq	%rsi, %r9
	jb	.L2
	leaq	-1(%rsi), %r8
	movq	%r9, %rax
	movq	%rdi, %rdx
	.p2align 4,,10
	.p2align 3
.L3:
	movzbl	(%rax), %ecx
	subq	$1, %rax
	addq	$1, %rdx
	movb	%cl, -1(%rdx)
	cmpq	%rax, %r8
	jne	.L3
	subq	%rsi, %r9
	leaq	1(%rdi,%r9), %rdi
.L2:
	movb	$0, (%rdi)
	movl	$1, %eax
	ret
	.cfi_endproc
.LFE24:
	.size	nixRev, .-nixRev
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"nixRevThis"
	.text
	.p2align 4
	.globl	nixRevSelf
	.type	nixRevSelf, @function
nixRevSelf:
.LFB25:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	movq	%rsi, %rdi
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	malloc@PLT
	testq	%rax, %rax
	je	.L13
	movq	%rbp, %rdx
	movq	%rbx, %rsi
	movq	%rax, %rdi
	call	memcpy@PLT
	leaq	-1(%rax,%rbp), %rdx
	cmpq	%rax, %rdx
	jb	.L9
	leaq	-1(%rax), %rax
	movq	%rbx, %rcx
	.p2align 4,,10
	.p2align 3
.L10:
	movzbl	(%rdx), %esi
	subq	$1, %rdx
	addq	$1, %rcx
	movb	%sil, -1(%rcx)
	cmpq	%rdx, %rax
	jne	.L10
	addq	%rbp, %rbx
.L9:
	movb	$0, (%rbx)
	movl	$1, %eax
.L6:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L13:
	.cfi_restore_state
	leaq	.LC0(%rip), %rdi
	call	perror@PLT
	xorl	%eax, %eax
	jmp	.L6
	.cfi_endproc
.LFE25:
	.size	nixRevSelf, .-nixRevSelf
	.p2align 4
	.globl	nixSizeOfFile
	.type	nixSizeOfFile, @function
nixSizeOfFile:
.LFB26:
	.cfi_startproc
	subq	$168, %rsp
	.cfi_def_cfa_offset 176
	movq	%fs:40, %rax
	movq	%rax, 152(%rsp)
	xorl	%eax, %eax
	movq	%rsp, %rsi
	call	stat@PLT
	movl	%eax, %edx
	xorl	%eax, %eax
	testl	%edx, %edx
	jne	.L14
	movq	48(%rsp), %rax
.L14:
	movq	152(%rsp), %rdx
	subq	%fs:40, %rdx
	jne	.L20
	addq	$168, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L20:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE26:
	.size	nixSizeOfFile, .-nixSizeOfFile
	.section	.rodata.str1.1
.LC1:
	.string	""
	.text
	.p2align 4
	.globl	nixTee
	.type	nixTee, @function
nixTee:
.LFB27:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %rbp
	movq	%rdx, %rdi
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	fopen@PLT
	testq	%rax, %rax
	je	.L25
	movq	%rax, %rbx
	movq	%rax, %rsi
	movq	%rbp, %rdi
	call	fputs@PLT
	movq	%rbx, %rdi
	call	fclose@PLT
	movl	$1, %eax
.L21:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L25:
	.cfi_restore_state
	leaq	.LC1(%rip), %rdi
	call	perror@PLT
	xorl	%eax, %eax
	jmp	.L21
	.cfi_endproc
.LFE27:
	.size	nixTee, .-nixTee
	.p2align 4
	.globl	nixFind
	.type	nixFind, @function
nixFind:
.LFB28:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	movq	%rsi, %rdi
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	opendir@PLT
	testq	%rax, %rax
	je	.L39
	movq	%rax, %rbp
	movq	%rbp, %rdi
	call	readdir@PLT
	testq	%rax, %rax
	je	.L40
	.p2align 4,,10
	.p2align 3
.L31:
	leaq	19(%rax), %rdx
	movzbl	19(%rax), %eax
	testb	%al, %al
	je	.L29
	.p2align 4,,10
	.p2align 3
.L30:
	addq	$1, %rdx
	movb	%al, (%rbx)
	addq	$1, %rbx
	movzbl	(%rdx), %eax
	testb	%al, %al
	jne	.L30
.L29:
	movb	$10, (%rbx)
	movq	%rbp, %rdi
	addq	$1, %rbx
	call	readdir@PLT
	testq	%rax, %rax
	jne	.L31
.L40:
	movq	%rbp, %rdi
	call	closedir@PLT
	movb	$0, -1(%rbx)
	movl	$1, %eax
.L26:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L39:
	.cfi_restore_state
	leaq	.LC1(%rip), %rdi
	call	perror@PLT
	xorl	%eax, %eax
	jmp	.L26
	.cfi_endproc
.LFE28:
	.size	nixFind, .-nixFind
	.p2align 4
	.globl	nixFindAuto
	.type	nixFindAuto, @function
nixFindAuto:
.LFB29:
	.cfi_startproc
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	movq	%rdi, %r12
	movq	%rsi, %rdi
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	subq	$8, %rsp
	.cfi_def_cfa_offset 48
	call	opendir@PLT
	testq	%rax, %rax
	je	.L42
	movl	$16000, %edi
	movq	%rax, %rbp
	call	malloc@PLT
	movq	%rax, (%r12)
	testq	%rax, %rax
	je	.L42
	movq	%rbp, %rdi
	xorl	%r13d, %r13d
	call	readdir@PLT
	movq	%rax, %rbx
	testq	%rax, %rax
	je	.L62
	.p2align 4,,10
	.p2align 3
.L47:
	leaq	19(%rbx), %rdi
	call	strlen@PLT
	testq	%rax, %rax
	jne	.L44
	movzbl	19(%rbx), %ecx
	movq	%rbx, %rax
	movq	(%r12), %rsi
	movq	%r13, %rdx
	subq	%r13, %rax
	testb	%cl, %cl
	je	.L46
	.p2align 4,,10
	.p2align 3
.L45:
	movb	%cl, (%rsi,%rdx)
	addq	$1, %rdx
	movzbl	19(%rax,%rdx), %ecx
	testb	%cl, %cl
	jne	.L45
.L46:
	movb	$10, (%rsi,%rdx)
	movq	%rbp, %rdi
	leaq	1(%rdx), %r13
	call	readdir@PLT
	movq	%rax, %rbx
	testq	%rax, %rax
	jne	.L47
.L62:
	movq	%rbp, %rdi
	call	closedir@PLT
	movq	(%r12), %rdi
	leaq	1(%r13), %rsi
	call	realloc@PLT
	movq	%rax, (%r12)
	testq	%rax, %rax
	je	.L42
	movb	$0, -1(%rax,%r13)
	leal	-1(%r13), %eax
.L41:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	ret
.L44:
	.cfi_restore_state
	jmp	.L44
	.p2align 4,,10
	.p2align 3
.L42:
	leaq	.LC1(%rip), %rdi
	call	perror@PLT
	xorl	%eax, %eax
	jmp	.L41
	.cfi_endproc
.LFE29:
	.size	nixFindAuto, .-nixFindAuto
	.section	.rodata.str1.1
.LC2:
	.string	"r"
	.text
	.p2align 4
	.globl	nixHead
	.type	nixHead, @function
nixHead:
.LFB30:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	movq	%rsi, %rdi
	leaq	.LC2(%rip), %rsi
	call	fopen@PLT
	testq	%rax, %rax
	je	.L64
	leaq	.LC1(%rip), %rdi
	call	perror@PLT
	xorl	%eax, %eax
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L64:
	.cfi_restore_state
	movq	%rbx, %rdi
	xorl	%edx, %edx
	movl	$256, %esi
	call	fgets@PLT
	xorl	%edi, %edi
	call	fclose@PLT
	movl	$1, %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE30:
	.size	nixHead, .-nixHead
	.p2align 4
	.globl	nixCat
	.type	nixCat, @function
nixCat:
.LFB31:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %rbp
	movq	%rsi, %rdi
	leaq	.LC2(%rip), %rsi
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdx, %rbx
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	fopen@PLT
	testq	%rax, %rax
	je	.L72
.L68:
	leaq	.LC1(%rip), %rdi
	call	perror@PLT
	xorl	%eax, %eax
.L67:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L72:
	.cfi_restore_state
	movq	%rbp, %rdi
	xorl	%ecx, %ecx
	movq	%rbx, %rdx
	movl	$1, %esi
	call	fread@PLT
	xorl	%edi, %edi
	testq	%rax, %rax
	je	.L73
	call	fclose@PLT
	movb	$0, 0(%rbp,%rbx)
	movl	$1, %eax
	jmp	.L67
.L73:
	call	fclose@PLT
	jmp	.L68
	.cfi_endproc
.LFE31:
	.size	nixCat, .-nixCat
	.p2align 4
	.globl	nixCatFast
	.type	nixCatFast, @function
nixCatFast:
.LFB32:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %rbp
	movq	%rsi, %rdi
	leaq	.LC2(%rip), %rsi
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdx, %rbx
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	fopen@PLT
	testq	%rax, %rax
	je	.L79
.L75:
	leaq	.LC1(%rip), %rdi
	call	perror@PLT
	xorl	%eax, %eax
.L74:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L79:
	.cfi_restore_state
	movq	%rbp, %rdi
	xorl	%ecx, %ecx
	movq	%rbx, %rdx
	movl	$1, %esi
	call	fread_unlocked@PLT
	xorl	%edi, %edi
	testq	%rax, %rax
	je	.L80
	call	fclose@PLT
	movb	$0, 0(%rbp,%rbx)
	movl	$1, %eax
	jmp	.L74
.L80:
	call	fclose@PLT
	jmp	.L75
	.cfi_endproc
.LFE32:
	.size	nixCatFast, .-nixCatFast
	.p2align 4
	.globl	nixCatAuto
	.type	nixCatAuto, @function
nixCatAuto:
.LFB33:
	.cfi_startproc
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movq	%rsi, %rbx
	movq	%rbx, %rdi
	subq	$160, %rsp
	.cfi_def_cfa_offset 192
	movq	%fs:40, %rax
	movq	%rax, 152(%rsp)
	xorl	%eax, %eax
	movq	%rsp, %rsi
	call	stat@PLT
	testl	%eax, %eax
	jne	.L83
	movq	48(%rsp), %r12
	testq	%r12, %r12
	jne	.L97
.L83:
	leaq	.LC1(%rip), %rdi
	call	perror@PLT
	xorl	%eax, %eax
.L81:
	movq	152(%rsp), %rdx
	subq	%fs:40, %rdx
	jne	.L98
	addq	$160, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L97:
	.cfi_restore_state
	movq	%rbx, %rdi
	leaq	.LC2(%rip), %rsi
	call	fopen@PLT
	movq	%rax, %rbx
	testq	%rax, %rax
	je	.L83
	movq	%r12, %rdi
	call	malloc@PLT
	movq	%rax, 0(%rbp)
	movq	%rax, %rdi
	testq	%rax, %rax
	je	.L86
	movq	%rbx, %rcx
	movq	%r12, %rdx
	movl	$1, %esi
	call	fread@PLT
	testq	%rax, %rax
	je	.L99
	movq	%rbx, %rdi
	call	fclose@PLT
	movq	0(%rbp), %rax
	movb	$0, (%rax,%r12)
	movl	$1, %eax
	jmp	.L81
	.p2align 4,,10
	.p2align 3
.L99:
	movq	0(%rbp), %rdi
	call	free@PLT
.L86:
	movq	%rbx, %rdi
	call	fclose@PLT
	jmp	.L83
.L98:
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE33:
	.size	nixCatAuto, .-nixCatAuto
	.p2align 4
	.globl	nixCatAutoFast
	.type	nixCatAutoFast, @function
nixCatAutoFast:
.LFB34:
	.cfi_startproc
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movq	%rsi, %rbx
	movq	%rbx, %rdi
	subq	$160, %rsp
	.cfi_def_cfa_offset 192
	movq	%fs:40, %rax
	movq	%rax, 152(%rsp)
	xorl	%eax, %eax
	movq	%rsp, %rsi
	call	stat@PLT
	testl	%eax, %eax
	jne	.L102
	movq	48(%rsp), %r12
	testq	%r12, %r12
	jne	.L116
.L102:
	leaq	.LC1(%rip), %rdi
	call	perror@PLT
	xorl	%eax, %eax
.L100:
	movq	152(%rsp), %rdx
	subq	%fs:40, %rdx
	jne	.L117
	addq	$160, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L116:
	.cfi_restore_state
	movq	%rbx, %rdi
	leaq	.LC2(%rip), %rsi
	call	fopen@PLT
	movq	%rax, %rbx
	testq	%rax, %rax
	je	.L102
	movq	%r12, %rdi
	call	malloc@PLT
	movq	%rax, 0(%rbp)
	movq	%rax, %rdi
	testq	%rax, %rax
	je	.L105
	movq	%rbx, %rcx
	movq	%r12, %rdx
	movl	$1, %esi
	call	fread_unlocked@PLT
	testq	%rax, %rax
	je	.L118
	movq	%rbx, %rdi
	call	fclose@PLT
	movq	0(%rbp), %rax
	movb	$0, (%rax,%r12)
	movl	$1, %eax
	jmp	.L100
	.p2align 4,,10
	.p2align 3
.L118:
	movq	0(%rbp), %rdi
	call	free@PLT
.L105:
	movq	%rbx, %rdi
	call	fclose@PLT
	jmp	.L102
.L117:
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE34:
	.size	nixCatAutoFast, .-nixCatAutoFast
	.p2align 4
	.globl	nixCutFirst
	.type	nixCutFirst, @function
nixCutFirst:
.LFB35:
	.cfi_startproc
	movabsq	$4294977025, %rdx
.L122:
	movzbl	(%rsi), %eax
	cmpb	$32, %al
	ja	.L120
	btq	%rax, %rdx
	jc	.L121
.L120:
	addq	$1, %rdi
	addq	$1, %rsi
	movb	%al, -1(%rdi)
	jmp	.L122
	.p2align 4,,10
	.p2align 3
.L121:
	movb	$0, (%rdi)
	movl	$1, %eax
	ret
	.cfi_endproc
.LFE35:
	.size	nixCutFirst, .-nixCutFirst
	.p2align 4
	.globl	nixCutFirstDelim
	.type	nixCutFirstDelim, @function
nixCutFirstDelim:
.LFB36:
	.cfi_startproc
	.p2align 4,,10
	.p2align 3
.L124:
	movsbl	(%rsi), %eax
	movq	%rdi, %rcx
	addq	$1, %rsi
	addq	$1, %rdi
	movb	%al, -1(%rdi)
	cmpl	%edx, %eax
	jne	.L124
	movb	$0, (%rcx)
	movl	$1, %eax
	ret
	.cfi_endproc
.LFE36:
	.size	nixCutFirstDelim, .-nixCutFirstDelim
	.p2align 4
	.globl	nixCutLast
	.type	nixCutLast, @function
nixCutLast:
.LFB37:
	.cfi_startproc
	leaq	-1(%rsi,%rdx), %rcx
	movabsq	$4294977024, %rdx
.L129:
	movzbl	(%rcx), %eax
	cmpb	$32, %al
	ja	.L127
	btq	%rax, %rdx
	jc	.L130
.L127:
	subq	$1, %rcx
	jmp	.L129
	.p2align 4,,10
	.p2align 3
.L130:
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L128:
	movzbl	1(%rcx,%rax), %edx
	movb	%dl, (%rdi,%rax)
	addq	$1, %rax
	testb	%dl, %dl
	jne	.L128
	movl	$1, %eax
	ret
	.cfi_endproc
.LFE37:
	.size	nixCutLast, .-nixCutLast
	.p2align 4
	.globl	nixCutLastDelim
	.type	nixCutLastDelim, @function
nixCutLastDelim:
.LFB38:
	.cfi_startproc
	leaq	-1(%rsi,%rdx), %rax
	.p2align 4,,10
	.p2align 3
.L133:
	movq	%rax, %rdx
	leaq	-1(%rax), %rax
	movsbl	(%rdx), %esi
	cmpl	%ecx, %esi
	jne	.L133
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L134:
	movzbl	1(%rdx,%rax), %ecx
	movb	%cl, (%rdi,%rax)
	addq	$1, %rax
	testb	%cl, %cl
	jne	.L134
	movl	$1, %eax
	ret
	.cfi_endproc
.LFE38:
	.size	nixCutLastDelim, .-nixCutLastDelim
	.p2align 4
	.globl	nixCutDelim
	.type	nixCutDelim, @function
nixCutDelim:
.LFB39:
	.cfi_startproc
	movsbl	(%rsi), %eax
	leal	-1(%rdx), %r8d
	testl	%edx, %edx
	je	.L143
	.p2align 4,,10
	.p2align 3
.L138:
	cmpl	%eax, %ecx
	je	.L142
	.p2align 4,,10
	.p2align 3
.L140:
	movsbl	1(%rsi), %eax
	addq	$1, %rsi
	cmpl	%ecx, %eax
	jne	.L140
	.p2align 4,,10
	.p2align 3
.L142:
	movsbl	1(%rsi), %eax
	addq	$1, %rsi
	cmpl	%ecx, %eax
	je	.L142
	subl	$1, %r8d
	jnb	.L138
	.p2align 4,,10
	.p2align 3
.L143:
	movsbl	(%rsi), %eax
	movq	%rdi, %rdx
	addq	$1, %rsi
	addq	$1, %rdi
	movb	%al, -1(%rdi)
	cmpl	%ecx, %eax
	jne	.L143
	movb	$0, (%rdx)
	movl	$1, %eax
	ret
	.cfi_endproc
.LFE39:
	.size	nixCutDelim, .-nixCutDelim
	.p2align 4
	.globl	nixCut
	.type	nixCut, @function
nixCut:
.LFB40:
	.cfi_startproc
.L154:
	movzbl	(%rsi), %eax
	addq	$1, %rsi
	cmpb	$10, %al
	jg	.L155
	cmpb	$8, %al
	jg	.L156
	testb	%al, %al
	jne	.L154
.L161:
	movzbl	(%rsi), %eax
	cmpb	$32, %al
	ja	.L159
	movabsq	$4294977025, %rdx
	btq	%rax, %rdx
	jc	.L160
.L159:
	addq	$1, %rdi
	addq	$1, %rsi
	movb	%al, -1(%rdi)
	jmp	.L161
	.p2align 4,,10
	.p2align 3
.L155:
	cmpb	$13, %al
	je	.L156
	cmpb	$32, %al
	jne	.L154
.L156:
	subl	$1, %edx
	je	.L161
	jmp	.L154
	.p2align 4,,10
	.p2align 3
.L160:
	movb	$0, (%rdi)
	movl	$1, %eax
	ret
	.cfi_endproc
.LFE40:
	.size	nixCut, .-nixCut
	.p2align 4
	.globl	nixCountFunc
	.type	nixCountFunc, @function
nixCountFunc:
.LFB41:
	.cfi_startproc
	xorl	%edx, %edx
	.p2align 4,,10
	.p2align 3
.L170:
	movsbl	(%rdi), %ecx
	cmpl	%ecx, %esi
	movl	%ecx, %eax
	sete	%cl
	addq	$1, %rdi
	movzbl	%cl, %ecx
	addl	%ecx, %edx
	testb	%al, %al
	jne	.L170
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE41:
	.size	nixCountFunc, .-nixCountFunc
	.p2align 4
	.globl	nixCountDigit
	.type	nixCountDigit, @function
nixCountDigit:
.LFB42:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	call	__ctype_b_loc@PLT
	xorl	%ecx, %ecx
	movq	(%rax), %rsi
	.p2align 4,,10
	.p2align 3
.L174:
	movsbq	(%rbx), %rax
	movq	%rax, %rdx
	movzwl	(%rsi,%rax,2), %eax
	andw	$2048, %ax
	cmpw	$1, %ax
	sbbl	$-1, %ecx
	addq	$1, %rbx
	testb	%dl, %dl
	jne	.L174
	movl	%ecx, %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE42:
	.size	nixCountDigit, .-nixCountDigit
	.p2align 4
	.globl	nixCountAlpha
	.type	nixCountAlpha, @function
nixCountAlpha:
.LFB43:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	call	__ctype_b_loc@PLT
	xorl	%ecx, %ecx
	movq	(%rax), %rsi
	.p2align 4,,10
	.p2align 3
.L181:
	movsbq	(%rbx), %rax
	movq	%rax, %rdx
	movzwl	(%rsi,%rax,2), %eax
	andw	$1024, %ax
	cmpw	$1, %ax
	sbbl	$-1, %ecx
	addq	$1, %rbx
	testb	%dl, %dl
	jne	.L181
	movl	%ecx, %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE43:
	.size	nixCountAlpha, .-nixCountAlpha
	.p2align 4
	.globl	nixWcWord
	.type	nixWcWord, @function
nixWcWord:
.LFB44:
	.cfi_startproc
	xorl	%ecx, %ecx
	xorl	%edx, %edx
.L191:
	movzbl	(%rdi), %eax
	cmpb	$10, %al
	jg	.L187
	cmpb	$8, %al
	jg	.L188
	testb	%al, %al
	jne	.L193
	cmpl	$1, %edx
	sbbl	$-1, %ecx
	movl	%ecx, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L187:
	cmpb	$13, %al
	je	.L188
	cmpb	$32, %al
	jne	.L193
.L188:
	testl	%edx, %edx
	je	.L190
	addl	$1, %ecx
	xorl	%edx, %edx
	.p2align 4,,10
	.p2align 3
.L190:
	addq	$1, %rdi
	jmp	.L191
	.p2align 4,,10
	.p2align 3
.L193:
	movl	$1, %edx
	jmp	.L190
	.cfi_endproc
.LFE44:
	.size	nixWcWord, .-nixWcWord
	.ident	"GCC: (GNU) 12.2.1 20230201"
	.section	.note.GNU-stack,"",@progbits
