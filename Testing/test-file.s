.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movl $80, -4(%rbp)
.Lloop.0:
	cmpl $100, -4(%rbp)
	movl $0, -8(%rbp)
	setl -8(%rbp)
	cmpl $0, -8(%rbp)
	je .Lexit.0
	movl -4(%rbp), %r10d
	movl %r10d, -12(%rbp)
	movl -12(%rbp), %ecx
	call putchar
	addq $-16, %rsp
	movl %eax, -16(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -20(%rbp)
	addl $1, -20(%rbp)
	movl -20(%rbp), %r10d
	movl %r10d, -4(%rbp)
	jmp .Lloop.0
.Lexit.0:
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret


