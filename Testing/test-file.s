.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movl $7, -4(%rbp)
	movl $60, -12(%rbp)
	movl $72, -16(%rbp)
.Lloop.0:
	cmpl $100, -16(%rbp)
	movl $0, -20(%rbp)
	setl -20(%rbp)
	cmpl $0, -20(%rbp)
	je .Lexit.0
	movl -12(%rbp), %r10d
	movl %r10d, -8(%rbp)
	movl -8(%rbp), %edi
	call putchar@PLT
	addq $-32, %rsp
	movl %eax, -24(%rbp)
	movl -16(%rbp), %r10d
	movl %r10d, -28(%rbp)
	addl $1, -28(%rbp)
	movl -28(%rbp), %r10d
	movl %r10d, -16(%rbp)
	jmp .Lloop.0
.Lexit.0:
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
