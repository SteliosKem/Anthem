.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $12, %rsp
	movl $4, -4(%rbp)
	cmpl $6, -4(%rbp)
	movl $0, -8(%rbp)
	sete -8(%rbp)
	cmpl $0, -8(%rbp)
	je .Lfalse_label.0
	movl $7, -4(%rbp)
	jmp .Lend_label.0
.Lfalse_label.0:
	cmpl $9, -4(%rbp)
	movl $0, -12(%rbp)
	setg -12(%rbp)
	cmpl $0, -12(%rbp)
	je .Lfalse_label.1
	movl $4, -4(%rbp)
	jmp .Lend_label.1
.Lfalse_label.1:
	movl $0, -4(%rbp)
.Lend_label.1:
.Lend_label.0:
	movl -4(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
