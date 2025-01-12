.globl other_func
other_func:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl %edi, -4(%rbp)
	movl %esi, -12(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -8(%rbp)
	movl -8(%rbp), %r10d
	subl -12(%rbp), %r10d
	movl %r10d, -8(%rbp)
	movl -8(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
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
	movl $6, -8(%rbp)
	movl -8(%rbp), %r10d
	movl %r10d, -12(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -16(%rbp)
	subq $0, %rsp
	movl -12(%rbp), %edi
	movl -16(%rbp), %esi
	call other_func
	addq $-32, %rsp
	movl %eax, -20(%rbp)
	movl -20(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
