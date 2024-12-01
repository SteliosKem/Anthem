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
		emit_string(".globl " + identifier);
		emit_line();
	}

	void x86_GAS_Emitter::emit_function_prologue() {}

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

	void x86_GAS_Emitter::emit_operand(ptr<ASMOperandNode> operand) {
		switch (operand->get_type())
		{
		case ASMNodeType::REGISTER:
			emit_register(std::static_pointer_cast<RegisterOperandNode>(operand)->register_op);
			return;
		case ASMNodeType::INTEGER:
			emit_integer(std::static_pointer_cast<IntegerOperandNode>(operand)->integer);
			break;
		default:
			break;
		}
	}

	void x86_GAS_Emitter::emit_register(Register register_op) {
		switch (register_op) {
		case Register::EAX:
			emit_string("%eax", false);
			break;
		}
	}

	void x86_GAS_Emitter::emit_integer(int integer) {
		emit_string(std::format("&{0}", integer), false);
	}

	void x86_GAS_Emitter::emit_return() {
		emit_string("ret");
		emit_line();
	}

	void x86_GAS_Emitter::emit_file_epilogue() {
		emit_string(".section .note.GNU-stack,\"\",@progbits", false);
	}

	const std::string& x86_GAS_Emitter::assembly_out() {
		return m_assembly_output;
	}
}