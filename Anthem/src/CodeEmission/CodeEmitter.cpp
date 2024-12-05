#include "CodeEmitter.h"

namespace Anthem {
	void CodeEmitter::emit(ptr<ASMProgramNode> program_node, std::string& assembly) {
		emit_program(program_node);
		emit_file_epilogue();
		assembly = assembly_out();
	}

	void CodeEmitter::emit_program(ptr<ASMProgramNode> program) {
		for (auto& decl : program->declarations) {
			emit_declaration(decl);
		}
	}

	void CodeEmitter::emit_function(ptr<ASMFunctionNode> function) {
		emit_global_identifier(function->name);
		emit_label(function->name);
		emit_function_prologue();
		for(auto& instruction : function->instructions)
			emit_instruction(instruction);
		emit_function_epilogue();
	}

	void CodeEmitter::emit_declaration(ptr<ASMDeclarationNode> declaration) {
		switch (declaration->get_type())
		{
		case ASMNodeType::FUNCTION:
			emit_function(std::static_pointer_cast<ASMFunctionNode>(declaration));
			break;
		default:
			break;
		}
	}

	void CodeEmitter::emit_operand(ptr<ASMOperandNode> operand) {
		switch (operand->get_type())
		{
		case ASMNodeType::REGISTER:
			emit_register(std::static_pointer_cast<RegisterOperandNode>(operand)->register_op);
			return;
		case ASMNodeType::INTEGER:
			emit_integer(std::static_pointer_cast<IntegerOperandNode>(operand)->integer);
			break;
		case ASMNodeType::STACK_OPERAND:
			emit_stack_access(std::static_pointer_cast<StackOperandNode>(operand)->position);
			break;
		case ASMNodeType::PSEUDO_OPERAND:
			emit_stack_access(std::static_pointer_cast<PseudoOperandNode>(operand)->stack_offset);
			break;
		default:
			break;
		}
	}

	void CodeEmitter::emit_instruction(ptr<ASMInstructionNode> instruction) {
		switch (instruction->get_type()) {
		case ASMNodeType::MOVE: {
			emit_move(std::static_pointer_cast<MoveInstructionNode>(instruction));
			break;
		case ASMNodeType::RETURN:
			emit_return();
			break;
		case ASMNodeType::UNARY:
			emit_unary(std::static_pointer_cast<UnaryInstructionNode>(instruction));
			break;
		case ASMNodeType::BINARY:
			emit_binary(std::static_pointer_cast<BinaryInstructionNode>(instruction));
			break;
		case ASMNodeType::ALLOCATE_STACK:
			emit_allocate_stack(std::static_pointer_cast<AllocateStackNode>(instruction));
			break;
		case ASMNodeType::DIVIDE:
			emit_idiv(std::static_pointer_cast<DivideInstructionNode>(instruction));
			break;
		case ASMNodeType::SIGN_EXTEND:
			emit_cdq();
			break;
		default:
			return;
		}
		}
	}
}