.globl a
a:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movl $7, -8(%rbp)
	movl $60, -16(%rbp)
	movl $72, -20(%rbp)
.Lloop.0:
	cmpl $100, -20(%rbp)
	movl $0, -24(%rbp)
	setl -24(%rbp)
	cmpl $0, -24(%rbp)
	je .Lexit.0
	movl -16(%rbp), %r10d
	movl %r10d, -28(%rbp)
	movl -28(%rbp), %edi
	call putchar@PLT
	addq $-32, %rsp
	movl %eax, -4(%rbp)
	movl -20(%rbp), %r10d
	movl %r10d, -12(%rbp)
	addl $1, -12(%rbp)
	movl -12(%rbp), %r10d
	movl %r10d, -20(%rbp)
	jmp .Lloop.0
.Lexit.0:
	subq $0, %rsp
	call a
	addq $-48, %rsp
	movl %eax, -4(%rbp)
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
