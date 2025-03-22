.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	subq $0, %rsp
	call getchar
	addq $-32, %rsp
	movl %eax, -4(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -8(%rbp)
	movl -8(%rbp), %r10d
	movl %r10d, -12(%rbp)
	addl $1, -12(%rbp)
	movl -12(%rbp), %r10d
	movl %r10d, -8(%rbp)
	movl -8(%rbp), %r10d
	movl %r10d, -16(%rbp)
	movl -16(%rbp), %ecx
	call putchar
	addq $-16, %rsp
	movl %eax, -4(%rbp)
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret


