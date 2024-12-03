#include "CodeGenerator.h"

namespace Anthem {
	CodeGenerator::CodeGenerator(ErrorHandler* error_handler) : m_error_handler{ error_handler } {}

	ptr<ASMProgramNode> CodeGenerator::generate(ptr<AIRProgramNode> program) {
		ptr<ASMProgramNode> asm_program = generate_program(program);
		replace_pseudo_registers();
		validate_move_instructions(asm_program);
		return asm_program;
	}

	ptr<ASMProgramNode> CodeGenerator::generate_program(ptr<AIRProgramNode> program_node) {
		ptr<ASMProgramNode> asm_program_node = std::make_shared<ASMProgramNode>();

		// Generate ASM Declarations from every one of the AST Program Node
		for (auto& declaration : program_node->declarations)
			asm_program_node->declarations.push_back(generate_declaration(declaration));

		return asm_program_node;
	}

	ptr<ASMDeclarationNode> CodeGenerator::generate_declaration(ptr<AIRDeclarationNode> declaration_node) {
		// Handle any Declaration Case
		switch (declaration_node->get_type()) {
		case AIRNodeType::FUNCTION:
			return generate_function_declaration(std::static_pointer_cast<AIRFunctionNode>(declaration_node));
		default:
			return nullptr;
		}
	}

	ptr<ASMFunctionNode> CodeGenerator::generate_function_declaration(ptr<AIRFunctionNode> function_node) {
		ptr<ASMFunctionNode> asm_function_node = std::make_shared<ASMFunctionNode>();
		asm_function_node->name = function_node->name;
		// Push new vector for local variables
		m_functions.push_back({ &asm_function_node->instructions });
		for (auto& instruction : function_node->instructions)
			generate_instruction(instruction, asm_function_node->instructions);
		return asm_function_node;
	}

	void CodeGenerator::generate_instruction(ptr<AIRInstructionNode> instruction_node, ASMInstructionList& list_output) {
		switch (instruction_node->get_type()) {
		case AIRNodeType::RETURN:
			generate_return(std::static_pointer_cast<AIRReturnInstructionNode>(instruction_node), list_output);
			return;
		case AIRNodeType::UNARY_OPERATION:
			generate_unary(std::static_pointer_cast<AIRUnaryInstructionNode>(instruction_node), list_output);
			return;
		default:
			return;
		}
	}

	void CodeGenerator::generate_unary(ptr<AIRUnaryInstructionNode> unary_node, ASMInstructionList& list_output) {
		auto destination = resolve_value(unary_node->destination);
		list_output.push_back(move_instruction(resolve_value(unary_node->source), destination));
		list_output.push_back(std::make_shared<UnaryInstructionNode>(unary_node->operation, destination));
	}

	void CodeGenerator::generate_return(ptr<AIRReturnInstructionNode> return_node, ASMInstructionList& list_output) {
		// Move the result of the return expression to EAX register
		list_output.push_back(move_instruction(resolve_value(return_node->value), REGISTER(EAX)));
		// Return
		list_output.push_back(return_instruction());
	}

	ptr<ASMOperandNode> CodeGenerator::resolve_value(ptr<AIRValueNode> value) {
		switch (value->get_type())
		{
		case AIRNodeType::INTEGER:
			return std::make_shared<IntegerOperandNode>(std::static_pointer_cast<AIRIntegerValueNode>(value)->integer);
		case AIRNodeType::VARIABLE:
			return make_pseudo_register(std::static_pointer_cast<AIRVariableValueNode>(value));
		default:
			break;
		}
	}

	ptr<PseudoOperandNode> CodeGenerator::make_pseudo_register(ptr<AIRVariableValueNode> variable) {
		ptr<PseudoOperandNode> pseudo = std::make_shared<PseudoOperandNode>(variable->variable);
		auto& pseudo_list = m_functions[m_functions.size() - 1].pseudo_registers;
		if (pseudo_list.find(variable->variable) != pseudo_list.end())
			return pseudo_list[variable->variable];
		pseudo_list[variable->variable] = pseudo;
		return pseudo;
	}

	ptr<ASMOperandNode> CodeGenerator::resolve_expression(ptr<ExpressionNode> expression) {
		switch (expression->get_type())
		{
		case NodeType::INT_LITERAL:
			return std::make_shared<IntegerOperandNode>(std::static_pointer_cast<IntegerLiteralNode>(expression)->integer);
		default:
			break;
		}
	}

	ptr<MoveInstructionNode> CodeGenerator::move_instruction(ptr<ASMOperandNode> source, ptr<ASMOperandNode> destination) {
		return std::make_shared<MoveInstructionNode>(source, destination);
	}

	ptr<ReturnInstructionNode> CodeGenerator::return_instruction() {
		return std::make_shared<ReturnInstructionNode>();
	}

	void CodeGenerator::replace_pseudo_registers() {
		for (auto& function : m_functions) {
			// Allocate Stack Size 4 times the pseudo registers the function has (each one is 4 bytes)
			function.instructions->push_front(std::make_shared<AllocateStackNode>(function.pseudo_registers.size() * 4));
			int offset = -4;
			for (auto& [name, pointer] : function.pseudo_registers) {
				pointer->stack_offset = offset;
				offset -= 4;
			}
		}
	}

	void CodeGenerator::validate_move_instructions(ptr<ASMProgramNode> program_node) {
		for (auto& declaration : program_node->declarations) {
			switch (declaration->get_type())
			{
			case ASMNodeType::FUNCTION:
				ptr<ASMFunctionNode> function = std::static_pointer_cast<ASMFunctionNode>(declaration);
				int index = 0;
				while (index < function->instructions.size()) {
					auto instruction = function->instructions[index];
					switch (instruction->get_type()) {
					case ASMNodeType::MOVE:
						auto move_instruction = std::static_pointer_cast<MoveInstructionNode>(instruction);
						if (move_instruction->source->get_type() == ASMNodeType::PSEUDO_OPERAND
							&& move_instruction->destination->get_type() == ASMNodeType::PSEUDO_OPERAND) {
							auto destination = move_instruction->destination;
							move_instruction->destination = REGISTER(R10D);
							auto new_move_instruction = std::make_shared<MoveInstructionNode>(REGISTER(R10D), destination);
							auto begin = function->instructions.begin();
							function->instructions.insert(std::next(begin, index + 1), new_move_instruction);
						}
					}
					index++;
				}
			}
		}
	}
}