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
		emit_instruction(function->instruction);
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

	void CodeEmitter::emit_instruction(ptr<ASMInstructionNode> instruction) {
		switch (instruction->get_type()) {
		case ASMNodeType::INSTRUCTION_PACK: {
			ptr<InstructionPackNode> pack = std::static_pointer_cast<InstructionPackNode>(instruction);
			for (auto& instruction : pack->instruction_list)
				emit_instruction(instruction);
			break;
		}
		case ASMNodeType::MOVE: {
			emit_move(std::static_pointer_cast<MoveInstructionNode>(instruction));
			break;
		case ASMNodeType::RETURN:
			emit_return();
			break;
		default:
			return;
		}
		}
	}
}