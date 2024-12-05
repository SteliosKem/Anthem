.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $12, %rsp
	movl $4, -4(%rbp)
	addl $2, -4(%rbp)
	movl $5, -8(%rbp)
	movl -8(%rbp), %r10d
	imull -4(%rbp), %r10d
	movl %r10d, -8(%rbp)
	movl -8(%rbp), %eax
	cdq
	movl $3, %r10d
	idivl %r10d
	movl %edx, -12(%rbp)
	movl -12(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
