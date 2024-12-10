.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $12, %rsp
	movl $4, -4(%rbp)
	movl $0, -4(%rbp)
.Lcondition.0:
	cmpl $5, -4(%rbp)
	movl $0, -8(%rbp)
	setl -8(%rbp)
	cmpl $0, -8(%rbp)
	je .Lexit.0
	movl -4(%rbp), %r10d
	movl %r10d, -12(%rbp)
	addl $1, -12(%rbp)
	movl -12(%rbp), %r10d
	movl %r10d, -4(%rbp)
	jmp .Lcondition.0
.Lexit.0:
	movl -4(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
