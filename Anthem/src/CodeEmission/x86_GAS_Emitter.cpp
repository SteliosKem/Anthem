#include "x86_GAS_Emitter.h"

namespace Anthem {
	// Utility
	std::string condition_code(BinaryOperation condition) {
		switch (condition)
		{
		case BinaryOperation::EQUAL:
			return "e";
			break;
		case BinaryOperation::NOT_EQUAL:
			return "ne";
			break;
		case BinaryOperation::GREATER:
			return "g";
			break;
		case BinaryOperation::GREATER_EQUAL:
			return "ge";
			break;
		case BinaryOperation::LESS:
			return "l";
			break;
		case BinaryOperation::LESS_EQUAL:
			return "le";
			break;
		default:
			return "";
		}
	}

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

	void x86_GAS_Emitter::emit_register(Register register_op, Size size) {
		
		switch (register_op) {
			case Register::EAX: {
				switch (size)
				{
				case Anthem::Size::BYTE:
					emit_string("%al", false);
					break;
				case Anthem::Size::WORD:
					emit_string("%ah", false);
					break;
				case Anthem::Size::DWORD:
					emit_string("%eax", false);
					break;
				case Anthem::Size::QWORD:
					emit_string("%rax", false);
					break;
				default:
					break;
				}
				break;
			}
			case Register::EDX: {
				switch (size)
				{
				case Anthem::Size::BYTE:
					emit_string("%dl", false);
					break;
				case Anthem::Size::WORD:
					emit_string("%dh", false);
					break;
				case Anthem::Size::DWORD:
					emit_string("%edx", false);
					break;
				case Anthem::Size::QWORD:
					emit_string("%rdx", false);
					break;
				default:
					break;
				}
				break;
			}
			case Register::R10D: {
				switch (size)
				{
				case Anthem::Size::BYTE:
					emit_string("%r10b", false);
					break;
				case Anthem::Size::WORD:
					emit_string("%r10w", false);
					break;
				case Anthem::Size::DWORD:
					emit_string("%r10d", false);
					break;
				case Anthem::Size::QWORD:
					emit_string("%r10q", false);
					break;
				default:
					break;
				}
				break;
			}
			case Register::R11D: {
				switch (size)
				{
				case Anthem::Size::BYTE:
					emit_string("%r11b", false);
					break;
				case Anthem::Size::WORD:
					emit_string("%r11w", false);
					break;
				case Anthem::Size::DWORD:
					emit_string("%r11d", false);
					break;
				case Anthem::Size::QWORD:
					emit_string("%r11q", false);
					break;
				default:
					break;
				}
				break;
			}
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
		case UnaryOperation::NEGATE:
			emit_string("negl ");
			emit_operand(unary_operation->operand);
			break;
		case UnaryOperation::COMPLEMENT:
			emit_string("notl ");
			emit_operand(unary_operation->operand);
			break;
		}
		emit_line();
	}

	void x86_GAS_Emitter::emit_binary(ptr<BinaryInstructionNode> binary_operation) {
		switch (binary_operation->binary_operation) {
		case BinaryOperation::ADDITION:
			emit_string("addl ");
			emit_operand(binary_operation->operand_a);
			emit_string(", ", false);
			emit_operand(binary_operation->operand_b);
			break;
		case BinaryOperation::SUBTRACTION:
			emit_string("subl ");
			emit_operand(binary_operation->operand_a);
			emit_string(", ", false);
			emit_operand(binary_operation->operand_b);
			break;
		case BinaryOperation::MULTIPLICATION:
			emit_string("imull ");
			emit_operand(binary_operation->operand_a);
			emit_string(", ", false);
			emit_operand(binary_operation->operand_b);
			break;
		}
		emit_line();
	}
	void x86_GAS_Emitter::emit_idiv(ptr<DivideInstructionNode> divide) {
		emit_string("idivl ");
		emit_operand(divide->operand);
		emit_line();
	}

	void x86_GAS_Emitter::emit_compare(ptr<CompareInstructionNode> compare_operation) {
		emit_string("cmpl ");
		emit_operand(compare_operation->operand_a);
		emit_string(", ", false);
		emit_operand(compare_operation->operand_b);
		emit_line();
	}

	void x86_GAS_Emitter::emit_jump(ptr<JumpInstructionNode> jump) {
		emit_string("jmp .L" + jump->label);
		emit_line();
	}
	void x86_GAS_Emitter::emit_jump_conditional(ptr<JumpConditionalNode> conditional_jump) {
		emit_string("j" + condition_code(conditional_jump->condition) + " .L" + conditional_jump->label);
		emit_line();
	}

	void x86_GAS_Emitter::emit_set_conditional(ptr<SetConditionalNode> set_conditional) {
		emit_string("set" + condition_code(set_conditional->condition) + " ");
		emit_operand(set_conditional->operand);
		emit_line();
	}

	void x86_GAS_Emitter::emit_cdq() {
		emit_string("cdq");
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