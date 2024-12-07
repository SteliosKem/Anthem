.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl $2, -4(%rbp)
	addl $4, -4(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -8(%rbp)
	movl -8(%rbp), %r10d
	movl %r10d, -16(%rbp)
	addl $2, -16(%rbp)
	movl -16(%rbp), %r10d
	movl %r10d, -8(%rbp)
	movl $2, -12(%rbp)
	movl -12(%rbp), %r10d
	movl %r10d, -8(%rbp)
	movl -8(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
