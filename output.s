	.text
	.file	"Flec"
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	subq	$24, %rsp
	.cfi_offset %rbx, -24
	movl	$4, -20(%rbp)
	movl	$1078523331, -28(%rbp)          # imm = 0x4048F5C3
	movl	$0, -16(%rbp)
	movl	$3, -12(%rbp)
	movl	$5, -24(%rbp)
	movl	$.Lfmtstr, %edi
	movl	$.L__unnamed_1, %esi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	$.Lfmtstr.1, %edi
	movl	$.L__unnamed_2, %esi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	-20(%rbp), %esi
	movl	$.Lfmtint, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	jmp	.LBB0_1
	.p2align	4, 0x90
.LBB0_2:                                # %then
                                        #   in Loop: Header=BB0_1 Depth=1
	movq	%rsp, %rax
	leaq	-16(%rax), %rsp
	movl	$3, -16(%rax)
	movq	%rsp, %rax
	leaq	-16(%rax), %rsp
	movl	$5, -16(%rax)
.LBB0_5:                                # %loopcond
                                        #   in Loop: Header=BB0_1 Depth=1
	cmpl	$0, -12(%rbp)
	jle	.LBB0_7
.LBB0_1:                                # %loop
                                        # =>This Inner Loop Header: Depth=1
	movq	%rsp, %rbx
	leaq	-16(%rbx), %rsp
	movl	$1, -16(%rbx)
	movl	$.Lfmtstr.2, %edi
	movl	$.L__unnamed_3, %esi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	-12(%rbp), %esi
	movl	$.Lfmtint.3, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	xorl	%eax, %eax
	subl	-16(%rbp), %eax
	movl	%eax, -16(%rbx)
	movl	-16(%rbp), %eax
	movl	-20(%rbp), %ecx
	leal	-36(%rcx,%rax), %eax
	movl	%eax, -16(%rbx)
	decl	-12(%rbp)
	je	.LBB0_2
# %bb.3:                                # %else
                                        #   in Loop: Header=BB0_1 Depth=1
	cmpl	$1, -12(%rbp)
	jne	.LBB0_6
# %bb.4:                                # %then17
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	$.Lfmtstr.5, %edi
	movl	$.L__unnamed_4, %esi
	xorl	%eax, %eax
	callq	printf@PLT
	jmp	.LBB0_5
.LBB0_6:                                # %else18
                                        #   in Loop: Header=BB0_1 Depth=1
	cmpl	$0, -12(%rbp)
	jns	.LBB0_5
	.p2align	4, 0x90
.LBB0_7:                                # %loop28
                                        # =>This Inner Loop Header: Depth=1
	movl	-16(%rbp), %esi
	incl	%esi
	movl	%esi, -16(%rbp)
	movl	$.Lfmtint.6, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	cmpl	$5, -16(%rbp)
	jl	.LBB0_7
# %bb.8:                                # %afterloop30
	movl	$.Lfmtstr.7, %edi
	movl	$.L__unnamed_5, %esi
	xorl	%eax, %eax
	callq	printf@PLT
	xorl	%eax, %eax
	leaq	-8(%rbp), %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	.L__unnamed_1,@object           # @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"enter input"
	.size	.L__unnamed_1, 12

	.type	.Lfmtstr,@object                # @fmtstr
.Lfmtstr:
	.asciz	"%s\n"
	.size	.Lfmtstr, 4

	.type	.L__unnamed_2,@object           # @1
.L__unnamed_2:
	.asciz	"value of x is "
	.size	.L__unnamed_2, 15

	.type	.Lfmtstr.1,@object              # @fmtstr.1
.Lfmtstr.1:
	.asciz	"%s\n"
	.size	.Lfmtstr.1, 4

	.type	.Lfmtint,@object                # @fmtint
.Lfmtint:
	.asciz	"%d\n"
	.size	.Lfmtint, 4

	.type	.L__unnamed_3,@object           # @2
.L__unnamed_3:
	.asciz	"Countdown:"
	.size	.L__unnamed_3, 11

	.type	.Lfmtstr.2,@object              # @fmtstr.2
.Lfmtstr.2:
	.asciz	"%s\n"
	.size	.Lfmtstr.2, 4

	.type	.Lfmtint.3,@object              # @fmtint.3
.Lfmtint.3:
	.asciz	"%d\n"
	.size	.Lfmtint.3, 4

	.type	.L__unnamed_6,@object           # @3
.L__unnamed_6:
	.asciz	"count is zeroooooooooooo"
	.size	.L__unnamed_6, 25

	.type	.Lfmtstr.4,@object              # @fmtstr.4
.Lfmtstr.4:
	.asciz	"%s\n"
	.size	.Lfmtstr.4, 4

	.type	.L__unnamed_4,@object           # @4
.L__unnamed_4:
	.asciz	"Almost there!"
	.size	.L__unnamed_4, 14

	.type	.Lfmtstr.5,@object              # @fmtstr.5
.Lfmtstr.5:
	.asciz	"%s\n"
	.size	.Lfmtstr.5, 4

	.type	.Lfmtint.6,@object              # @fmtint.6
.Lfmtint.6:
	.asciz	"%d\n"
	.size	.Lfmtint.6, 4

	.type	.L__unnamed_5,@object           # @5
.L__unnamed_5:
	.asciz	"Done!"
	.size	.L__unnamed_5, 6

	.type	.Lfmtstr.7,@object              # @fmtstr.7
.Lfmtstr.7:
	.asciz	"%s\n"
	.size	.Lfmtstr.7, 4

	.section	".note.GNU-stack","",@progbits
