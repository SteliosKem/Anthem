.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $20, %rsp
	movl $4, -4(%rbp)
.Lloop.0:
.Lloop.1:
	movl -4(%rbp), %r10d
	movl %r10d, -8(%rbp)
	addl $1, -8(%rbp)
	movl -8(%rbp), %r10d
	movl %r10d, -4(%rbp)
	cmpl $5, -4(%rbp)
	movl $0, -12(%rbp)
	setg -12(%rbp)
	cmpl $0, -12(%rbp)
	je .Lfalse_label.0
	jmp .Lexit.1
.Lfalse_label.0:
	jmp .Lloop.1
.Lexit.1:
	movl -4(%rbp), %r10d
	movl %r10d, -16(%rbp)
	addl $1, -16(%rbp)
	movl -16(%rbp), %r10d
	movl %r10d, -4(%rbp)
	cmpl $100, -4(%rbp)
	movl $0, -20(%rbp)
	setg -20(%rbp)
	cmpl $0, -20(%rbp)
	je .Lfalse_label.1
	jmp .Lexit.0
.Lfalse_label.1:
	jmp .Lloop.0
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
