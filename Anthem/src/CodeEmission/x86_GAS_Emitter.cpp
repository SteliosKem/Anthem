#include "x86_GAS_Emitter.h"

namespace Anthem {
	void x86_GAS_Emitter::emit_string(const std::string& str, bool idented) {
		if (idented)
			m_assembly_output += '\t';
		m_assembly_output += str;
	}

	void x86_GAS_Emitter::emit_line() {
		m_assembly_output += '\n';
	}

	void x86_GAS_Emitter::emit_label(const std::string& name) {
		emit_string(name + ":", false);
		emit_line();
	}

	void x86_GAS_Emitter::emit_global_identifier(const std::string& identifier) {
		emit_string(".globl " + identifier, false);
		emit_line();
	}

	void x86_GAS_Emitter::emit_function_prologue() {
		emit_string("pushq %rbp");
		emit_line();
		emit_string("movq %rsp, %rbp");
		emit_line();
	}

	void x86_GAS_Emitter::emit_function_epilogue() {
		emit_line();
	}

	void x86_GAS_Emitter::emit_move(ptr<MoveInstructionNode> move_instruction) {
		emit_string("movl ");
		emit_operand(move_instruction->source);
		emit_string(", ", false);
		emit_operand(move_instruction->destination);
		emit_line();
	}

	void x86_GAS_Emitter::emit_register(Register register_op) {
		switch (register_op) {
		case Register::EAX:
			emit_string("%eax", false);
			break;
		case Register::R10D:
			emit_string("%r10d", false);
			break;
		}
	}

	void x86_GAS_Emitter::emit_integer(int integer) {
		emit_string(std::format("${0}", integer), false);
	}

	void x86_GAS_Emitter::emit_stack_access(int offset) {
		emit_string(std::format("{0}(%rbp)", offset), false);
	}

	void x86_GAS_Emitter::emit_return() {
		emit_string("movq %rbp, %rsp");
		emit_line();
		emit_string("popq %rbp");
		emit_line();
		emit_string("ret");
		emit_line();
	}

	void x86_GAS_Emitter::emit_unary(ptr<UnaryInstructionNode> unary_operation) {
		switch (unary_operation->unary_operation) {
		case NEGATE:
			emit_string("negl ");
			emit_operand(unary_operation->operand);
			break;
		case COMPLEMENT:
			emit_string("notl ");
			emit_operand(unary_operation->operand);
			break;
		}
		emit_line();
	}

	void x86_GAS_Emitter::emit_allocate_stack(ptr<AllocateStackNode> allocate) {
		emit_string(std::format("subq ${0}, %rsp", allocate->position));
		emit_line();
	}

	void x86_GAS_Emitter::emit_file_epilogue() {
		emit_string(".section .note.GNU-stack,\"\",@progbits", false);
		emit_line();
	}

	const std::string& x86_GAS_Emitter::assembly_out() {
		return m_assembly_output;
	}
}