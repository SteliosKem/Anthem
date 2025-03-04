// CodeEmitter.h
// Contains the CodeEmitter Class method implementations
// Copyright (c) 2024-present, Stylianos Kementzetzidis

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

	void CodeEmitter::emit_operand(ptr<ASMOperandNode> operand, Size size) {
		switch (operand->get_type())
		{
		case ASMNodeType::REGISTER:
			emit_register(std::static_pointer_cast<RegisterOperandNode>(operand)->register_op, size);
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
		case ASMNodeType::JUMP:
			emit_jump(std::static_pointer_cast<JumpInstructionNode>(instruction));
			break;
		case ASMNodeType::JUMP_CONDITIONAL:
			emit_jump_conditional(std::static_pointer_cast<JumpConditionalNode>(instruction));
			break;
		case ASMNodeType::SET_CONDITIONAL:
			emit_set_conditional(std::static_pointer_cast<SetConditionalNode>(instruction));
			break;
		case ASMNodeType::COMPARE:
			emit_compare(std::static_pointer_cast<CompareInstructionNode>(instruction));
			break;
		case ASMNodeType::LABEL:
			emit_label(".L" + std::static_pointer_cast<ASMLabelNode>(instruction)->label);
			break;
		case ASMNodeType::SIGN_EXTEND:
			emit_cdq();
			break;
		case ASMNodeType::DEALLOCATE_STACK:
			emit_deallocate_stack(std::static_pointer_cast<ASMDeallocateStackNode>(instruction));
			break;
		case ASMNodeType::PUSH:
			emit_push_stack(std::static_pointer_cast<ASMPushStackNode>(instruction));
			break;
		case ASMNodeType::CALL:
			emit_call(std::static_pointer_cast<ASMCallNode>(instruction));
			break;
		default:
			return;
		}
		}
	}
}