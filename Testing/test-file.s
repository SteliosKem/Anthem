.globl putchar
putchar:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl %edi, -4(%rbp)
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

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
	movl $7, -4(%rbp)
	movl $60, -12(%rbp)
	movl $72, -8(%rbp)
.Lloop.0:
	cmpl $100, -8(%rbp)
	movl $0, -16(%rbp)
	setl -16(%rbp)
	cmpl $0, -16(%rbp)
	je .Lexit.0
	movl -12(%rbp), %r10d
	movl %r10d, -20(%rbp)
	movl -20(%rbp), %edi
	call putchar
	addq $-32, %rsp
	movl %eax, -24(%rbp)
	movl -8(%rbp), %r10d
	movl %r10d, -28(%rbp)
	addl $1, -28(%rbp)
	movl -28(%rbp), %r10d
	movl %r10d, -8(%rbp)
	jmp .Lloop.0
.Lexit.0:
	subq $0, %rsp
	call a
	addq $-48, %rsp
	movl %eax, -24(%rbp)
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
