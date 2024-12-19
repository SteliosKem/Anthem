.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $12, %rsp
	movl $4, -4(%rbp)
	cmpl $4, -4(%rbp)
	movl $0, -8(%rbp)
	sete -8(%rbp)
	cmpl $0, -8(%rbp)
	je .Lfalse_label.0
	movl $5, -12(%rbp)
.Lfalse_label.0:
	movl -4(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.globl other_func
other_func:
	pushq %rbp
	movq %rsp, %rbp
	subq $4, %rsp
	movl -4(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
