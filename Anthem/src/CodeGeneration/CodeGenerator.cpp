#include "CodeGenerator.h"

namespace Anthem {
	// Util
	bool is_complex_binary(BinaryOperation operation) {
		switch (operation)
		{
		case BinaryOperation::GREATER:
		case BinaryOperation::LESS:
		case BinaryOperation::GREATER_EQUAL:
		case BinaryOperation::LESS_EQUAL:
		case BinaryOperation::EQUAL:
		case BinaryOperation::NOT_EQUAL:
			return true;
		default:
			return false;
		}
	}

	CodeGenerator::CodeGenerator(ErrorHandler* error_handler) : m_error_handler{ error_handler } {}

	ptr<ASMProgramNode> CodeGenerator::generate(ptr<AIRProgramNode> program) {
		ptr<ASMProgramNode> asm_program = generate_program(program);
		replace_pseudo_registers();
		validate_instructions(asm_program);
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
		case AIRNodeType::BINARY_OPERATION:
			generate_binary(std::static_pointer_cast<AIRBinaryInstructionNode>(instruction_node), list_output);
			return;
		case AIRNodeType::JUMP:
			generate_jump(std::static_pointer_cast<AIRJumpInstructionNode>(instruction_node), list_output);
			return;
		case AIRNodeType::JUMP_IF_NOT_ZERO:
			generate_jump_if_not_zero(std::static_pointer_cast<AIRJumpIfNotZeroInstructionNode>(instruction_node), list_output);
			return;
		case AIRNodeType::JUMP_IF_ZERO:
			generate_jump_if_zero(std::static_pointer_cast<AIRJumpIfZeroInstructionNode>(instruction_node), list_output);
			return;
		case AIRNodeType::LABEL:
			generate_label(std::static_pointer_cast<AIRLabelNode>(instruction_node), list_output);
			return;
		case AIRNodeType::SET: {
			auto set = std::static_pointer_cast<AIRSetInstructionNode>(instruction_node);
			list_output.push_back(mov(resolve_value(set->value), resolve_value(set->variable)));
			return;
		}
		default:
			return;
		}
	}

	void CodeGenerator::generate_unary(ptr<AIRUnaryInstructionNode> unary_node, ASMInstructionList& list_output) {
		auto destination = resolve_value(unary_node->destination);
		auto source = resolve_value(unary_node->source);

		if (unary_node->operation != UnaryOperation::NOT) {
			list_output.push_back(mov(source, destination));
			list_output.push_back(std::make_shared<UnaryInstructionNode>(unary_node->operation, destination));
			return;
		}

		list_output.push_back(cmp(integer(0), source));
		list_output.push_back(mov(integer(0), destination));
		list_output.push_back(set(BinaryOperation::EQUAL, destination));
	}

	void CodeGenerator::handle_complex_binary(ptr<AIRBinaryInstructionNode> binary_node, ASMInstructionList& list_output) {
		auto destination = resolve_value(binary_node->destination);
		auto source_a = resolve_value(binary_node->source_a);
		auto source_b = resolve_value(binary_node->source_b);

		list_output.push_back(cmp(source_b, source_a));
		list_output.push_back(mov(integer(0), destination));

		switch (binary_node->operation)
		{
		case BinaryOperation::GREATER:
			list_output.push_back(std::make_shared<SetConditionalNode>(BinaryOperation::GREATER, destination));
			break;
		case BinaryOperation::LESS:
			list_output.push_back(std::make_shared<SetConditionalNode>(BinaryOperation::LESS, destination));
			break;
		case BinaryOperation::GREATER_EQUAL:
			list_output.push_back(std::make_shared<SetConditionalNode>(BinaryOperation::GREATER_EQUAL, destination));
			break;
		case BinaryOperation::LESS_EQUAL:
			list_output.push_back(std::make_shared<SetConditionalNode>(BinaryOperation::LESS_EQUAL, destination));
			break;
		case BinaryOperation::EQUAL:
			list_output.push_back(std::make_shared<SetConditionalNode>(BinaryOperation::EQUAL, destination));
			break;
		case BinaryOperation::NOT_EQUAL:
			list_output.push_back(std::make_shared<SetConditionalNode>(BinaryOperation::NOT_EQUAL, destination));
			break;
		default:
			break;
		}
	}

	void CodeGenerator::generate_binary(ptr<AIRBinaryInstructionNode> binary_node, ASMInstructionList& list_output) {
		// Handle logical and relational operations
		if (is_complex_binary(binary_node->operation)) {
			handle_complex_binary(binary_node, list_output);
			return;
		}

		auto destination = resolve_value(binary_node->destination);
		auto source_a = resolve_value(binary_node->source_a);
		auto source_b = resolve_value(binary_node->source_b);

		if (binary_node->operation == BinaryOperation::DIVISION) {
			list_output.push_back(mov(source_a, REGISTER(EAX)));
			list_output.push_back(sign_extend());
			list_output.push_back(div(source_b));
			list_output.push_back(mov(REGISTER(EAX), destination));
			return;
		}
		if (binary_node->operation == BinaryOperation::REMAINDER) {
			list_output.push_back(mov(source_a, REGISTER(EAX)));
			list_output.push_back(sign_extend());
			list_output.push_back(div(source_b));
			list_output.push_back(mov(REGISTER(EDX), destination));
			return;
		}
		list_output.push_back(mov(source_a, destination));
		list_output.push_back(std::make_shared<BinaryInstructionNode>(binary_node->operation, source_b, destination));
	}

	void CodeGenerator::generate_return(ptr<AIRReturnInstructionNode> return_node, ASMInstructionList& list_output) {
		// Move the result of the return expression to EAX register
		list_output.push_back(mov(resolve_value(return_node->value), REGISTER(EAX)));
		// Return
		list_output.push_back(ret());
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

	ptr<MoveInstructionNode> CodeGenerator::mov(ptr<ASMOperandNode> source, ptr<ASMOperandNode> destination) {
		return std::make_shared<MoveInstructionNode>(source, destination);
	}

	ptr<ReturnInstructionNode> CodeGenerator::ret() {
		return std::make_shared<ReturnInstructionNode>();
	}

	ptr<CompareInstructionNode> CodeGenerator::cmp(ptr<ASMOperandNode> source, ptr<ASMOperandNode> destination) {
		return std::make_shared<CompareInstructionNode>(source, destination);
	}

	ptr<SetConditionalNode> CodeGenerator::set(BinaryOperation operation, ptr<ASMOperandNode> destination) {
		return std::make_shared<SetConditionalNode>(operation, destination);
	}

	ptr<IntegerOperandNode> CodeGenerator::integer(int integer) {
		return std::make_shared<IntegerOperandNode>(integer);
	}

	ptr<SignExtendInstructionNode> CodeGenerator::sign_extend() {
		return std::make_shared<SignExtendInstructionNode>();
	}

	ptr<DivideInstructionNode> CodeGenerator::div(ptr<ASMOperandNode> operand) {
		return std::make_shared<DivideInstructionNode>(operand);
	}

	void CodeGenerator::generate_jump(ptr<AIRJumpInstructionNode> jump_node, ASMInstructionList& list_output) {
		list_output.push_back(std::make_shared<JumpInstructionNode>(jump_node->label));
	}

	void CodeGenerator::generate_jump_if_zero(ptr<AIRJumpIfZeroInstructionNode> jump_node, ASMInstructionList& list_output) {
		list_output.push_back(std::make_shared<CompareInstructionNode>(std::make_shared<IntegerOperandNode>(0), resolve_value(jump_node->condition)));
		list_output.push_back(std::make_shared<JumpConditionalNode>(BinaryOperation::EQUAL, jump_node->label));
	}

	void CodeGenerator::generate_jump_if_not_zero(ptr<AIRJumpIfNotZeroInstructionNode> jump_node, ASMInstructionList& list_output) {
		list_output.push_back(std::make_shared<CompareInstructionNode>(std::make_shared<IntegerOperandNode>(0), resolve_value(jump_node->condition)));
		list_output.push_back(std::make_shared<JumpConditionalNode>(BinaryOperation::NOT_EQUAL, jump_node->label));
	}

	void CodeGenerator::generate_label(ptr<AIRLabelNode> label_node, ASMInstructionList& list_output) {
		list_output.push_back(std::make_shared<ASMLabelNode>(label_node->label));
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

	void CodeGenerator::validate_instructions(ptr<ASMProgramNode> program_node) {
		for (auto& declaration : program_node->declarations) {
			switch (declaration->get_type())
			{
			case ASMNodeType::FUNCTION:
				ptr<ASMFunctionNode> function = std::static_pointer_cast<ASMFunctionNode>(declaration);
				int index = 0;
				while (index < function->instructions.size()) {
					auto instruction = function->instructions[index];
					switch (instruction->get_type()) {
					// Validate Move instructions
						case ASMNodeType::MOVE: {
							auto move_instruction = std::static_pointer_cast<MoveInstructionNode>(instruction);
							// If both operands are stack memory addresses, move value from first address to a scratch register
							// and then move the value from the scratch register to the other memory address
							if (move_instruction->source->get_type() == ASMNodeType::PSEUDO_OPERAND
								&& move_instruction->destination->get_type() == ASMNodeType::PSEUDO_OPERAND) {
								auto destination = move_instruction->destination;
								move_instruction->destination = REGISTER(R10D);
								auto new_move_instruction = std::make_shared<MoveInstructionNode>(REGISTER(R10D), destination);
								auto begin = function->instructions.begin();
								function->instructions.insert(std::next(begin, index + 1), new_move_instruction);
							}
							break;
						}
						case ASMNodeType::COMPARE: {
							auto cmp_instruction = std::static_pointer_cast<CompareInstructionNode>(instruction);
							// If both operands are stack memory addresses, move value from first address to a scratch register
							// and then move the value from the scratch register to the other memory address
							if (cmp_instruction->operand_b->get_type() == ASMNodeType::INTEGER) {
								auto destination = cmp_instruction->operand_b;
								cmp_instruction->operand_b = REGISTER(R11D);
								auto new_move_instruction = std::make_shared<MoveInstructionNode>(destination, REGISTER(R11D));
								auto begin = function->instructions.begin();
								function->instructions.insert(std::next(begin, index), new_move_instruction);
							}
							if (cmp_instruction->operand_a->get_type() == ASMNodeType::PSEUDO_OPERAND
								&& cmp_instruction->operand_b->get_type() == ASMNodeType::PSEUDO_OPERAND) {
								auto source = cmp_instruction->operand_a;
								cmp_instruction->operand_a = REGISTER(R10D);
								auto new_move_instruction = std::make_shared<MoveInstructionNode>(source, REGISTER(R10D));
								auto begin = function->instructions.begin();
								function->instructions.insert(std::next(begin, index), new_move_instruction);
							}
							break;
						}
						case ASMNodeType::BINARY: {
							auto binary_instruction = std::static_pointer_cast<BinaryInstructionNode>(instruction);
							// If both operands are stack memory addresses, move value from first address to a scratch register
							// and then move the value from the scratch register to the other memory address
							if (binary_instruction->operand_a->get_type() == ASMNodeType::PSEUDO_OPERAND
								&& binary_instruction->operand_b->get_type() == ASMNodeType::PSEUDO_OPERAND) {
								auto destination = binary_instruction->operand_b;

								// Move the second operand (which is also the destination) to a scratch register in order to operate on it
								auto first_move_instruction = std::make_shared<MoveInstructionNode>(destination, REGISTER(R10D));
								auto begin = function->instructions.begin();
								function->instructions.insert(std::next(begin, index), first_move_instruction);

								// After the operation, move the the value held by the scratch register to the original destination
								begin = function->instructions.begin();
								auto second_move_instruction = std::make_shared<MoveInstructionNode>(REGISTER(R10D), destination);
								function->instructions.insert(std::next(begin, index + 2), second_move_instruction);

								// Change the destination of the operation to the scratch register
								binary_instruction->operand_b = REGISTER(R10D);
							}
							if (binary_instruction->binary_operation == BinaryOperation::MULTIPLICATION
								&& binary_instruction->operand_b->get_type() == ASMNodeType::PSEUDO_OPERAND) {
								auto destination = binary_instruction->operand_b;
								auto set_move_instruction = std::make_shared<MoveInstructionNode>(destination, REGISTER(R11D));
								auto begin = function->instructions.begin();
								function->instructions.insert(std::next(begin, index), set_move_instruction);
								binary_instruction->operand_b = REGISTER(R11D);
								begin = function->instructions.begin();
								auto retrieve_move_instruction = std::make_shared<MoveInstructionNode>(REGISTER(R11D), destination);
								function->instructions.insert(std::next(begin, index + 2), retrieve_move_instruction);
							}
							break;
						}
						case ASMNodeType::DIVIDE: {
							auto div_instruction = std::static_pointer_cast<DivideInstructionNode>(instruction);
							// If div instruction operates on an integer, move the value to a scratch register
							// and perform the instruction on that
							if (div_instruction->operand->get_type() == ASMNodeType::INTEGER) {
								auto begin = function->instructions.begin();
								auto source = div_instruction->operand;
								auto new_move_instruction = std::make_shared<MoveInstructionNode>(source, REGISTER(R10D));
								function->instructions.insert(std::next(begin, index), new_move_instruction);
								div_instruction->operand = REGISTER(R10D);
							}
							break;
						}
					}
					index++;
				}
			}
		}
	}
}