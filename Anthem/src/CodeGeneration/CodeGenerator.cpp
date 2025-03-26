// CodeGenerator.cpp
// Contains the CodeGenerator Class method implementations
// Copyright (c) 2024-present, Stylianos Kementzetzidis

#include "CodeGenerator.h"

namespace Anthem {
	// Util
#define instr(x) list_output.push_back(x)
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

	CodeGenerator::CodeGenerator(ErrorHandler* error_handler, bool compile_for_windows)
		: m_error_handler{ error_handler }, m_compile_for_windows{ compile_for_windows } {}

	ptr<ASMProgramNode> CodeGenerator::generate(ptr<AIRProgramNode> program, std::vector<ptr<AIRFlaggedVarNode>>& flagged_vars) {
		ptr<ASMProgramNode> asm_program = generate_program(program, flagged_vars);
		replace_pseudo_registers();
		validate_instructions(asm_program);
		return asm_program;
	}

	ptr<ASMProgramNode> CodeGenerator::generate_program(ptr<AIRProgramNode> program_node, std::vector<ptr<AIRFlaggedVarNode>>& flagged_vars) {
		ptr<ASMProgramNode> asm_program_node = std::make_shared<ASMProgramNode>();

		for (auto& var : flagged_vars)
			asm_program_node->declarations.push_back(generate_flagged_var(var));

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
		case AIRNodeType::FLAGGED_VAR:
			return generate_flagged_var(std::static_pointer_cast<AIRFlaggedVarNode>(declaration_node));
		default:
			return nullptr;
		}
	}

	ptr<ASMFunctionNode> CodeGenerator::generate_function_declaration(ptr<AIRFunctionNode> function_node) {
		// System V Calling Convention sets 6 registers before allocating passed arguments to stack, Microsoft's requires 4
		uint8_t registers_to_set = (m_compile_for_windows ? 4 : 6);
		using R = Register;

		std::vector<Register> argument_registers = (m_compile_for_windows)
			? std::vector<Register>{ R::ECX, R::EDX, R::R8D, R::R9D }	// <- Microsoft ABI
			: std::vector<Register>{ R::EDI, R::ESI, R::EDX, R::ECX, R::R8D, R::R9D };	// <- System V ABI

		ptr<ASMFunctionNode> asm_function_node = std::make_shared<ASMFunctionNode>();
		asm_function_node->name = function_node->name;
		asm_function_node->flag = function_node->flag;
		// Push new vector for local variables
		m_functions.push_back({ &asm_function_node->instructions });
		for (uint32_t i = 0; i < function_node->parameters.size(); i++) {
			if(i <= 6)
				asm_function_node->instructions.push_back(
					mov(std::make_shared<RegisterOperandNode>(argument_registers[i]), make_pseudo_register(function_node->parameters[i])));
			else
				mov(stack(16 + (i - registers_to_set) * 8), make_pseudo_register(function_node->parameters[i]));
						// Stack access starts at 16, and for every argument after the 6th (The number of Registers used for Argument passing)
						// access a multiple of 8 bytes after 16
		}
		for (auto& instruction : function_node->instructions)
			generate_instruction(instruction, asm_function_node->instructions);
		return asm_function_node;
	}

	ptr<ASMFlaggedVar> CodeGenerator::generate_flagged_var(ptr<AIRFlaggedVarNode> var_node) {
		return std::make_shared<ASMFlaggedVar>(var_node->name, var_node->flag, var_node->initializer);
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
			instr(mov(resolve_value(set->value), resolve_value(set->variable)));
			return;
		}
		case AIRNodeType::CALL: {
			generate_call(std::static_pointer_cast<AIRFunctionCallNode>(instruction_node), list_output);
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
			instr(mov(source, destination));
			instr(std::make_shared<UnaryInstructionNode>(unary_node->operation, destination));
			return;
		}

		instr(cmp(integer(0), source));
		instr(mov(integer(0), destination));
		instr(set(BinaryOperation::EQUAL, destination));
	}

	void CodeGenerator::handle_complex_binary(ptr<AIRBinaryInstructionNode> binary_node, ASMInstructionList& list_output) {
		auto destination = resolve_value(binary_node->destination);
		auto source_a = resolve_value(binary_node->source_a);
		auto source_b = resolve_value(binary_node->source_b);

		instr(cmp(source_b, source_a));
		instr(mov(integer(0), destination));

		switch (binary_node->operation)
		{
		case BinaryOperation::GREATER:
			instr(std::make_shared<SetConditionalNode>(BinaryOperation::GREATER, destination));
			break;
		case BinaryOperation::LESS:
			instr(std::make_shared<SetConditionalNode>(BinaryOperation::LESS, destination));
			break;
		case BinaryOperation::GREATER_EQUAL:
			instr(std::make_shared<SetConditionalNode>(BinaryOperation::GREATER_EQUAL, destination));
			break;
		case BinaryOperation::LESS_EQUAL:
			instr(std::make_shared<SetConditionalNode>(BinaryOperation::LESS_EQUAL, destination));
			break;
		case BinaryOperation::EQUAL:
			instr(std::make_shared<SetConditionalNode>(BinaryOperation::EQUAL, destination));
			break;
		case BinaryOperation::NOT_EQUAL:
			instr(std::make_shared<SetConditionalNode>(BinaryOperation::NOT_EQUAL, destination));
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
			instr(mov(source_a, REGISTER(EAX)));
			instr(sign_extend());
			instr(div(source_b));
			instr(mov(REGISTER(EAX), destination));
			return;
		}
		if (binary_node->operation == BinaryOperation::REMAINDER) {
			instr(mov(source_a, REGISTER(EAX)));
			instr(sign_extend());
			instr(div(source_b));
			instr(mov(REGISTER(EDX), destination));
			return;
		}
		instr(mov(source_a, destination));
		instr(std::make_shared<BinaryInstructionNode>(binary_node->operation, source_b, destination));
	}

	void CodeGenerator::generate_return(ptr<AIRReturnInstructionNode> return_node, ASMInstructionList& list_output) {
		// Move the result of the return expression to EAX register
		instr(mov(resolve_value(return_node->value), REGISTER(EAX)));
		// Return
		instr(ret());
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
		if (variable->flagged) {
			pseudo->flagged = true;
			auto& flagged_list = m_functions[m_functions.size() - 1].flagged_vars;
			if (flagged_list.find(variable->variable) != flagged_list.end())
				return flagged_list[variable->variable];
			flagged_list[variable->variable] = pseudo;
		}
		auto& pseudo_list = m_functions[m_functions.size() - 1].pseudo_registers;
		if (pseudo_list.find(variable->variable) != pseudo_list.end())
			return pseudo_list[variable->variable];
		pseudo_list[variable->variable] = pseudo;
		return pseudo;
	}

	ptr<PseudoOperandNode> CodeGenerator::make_pseudo_register(const Name& variable) {
		ptr<PseudoOperandNode> pseudo = std::make_shared<PseudoOperandNode>(variable);
		auto& pseudo_list = m_functions[m_functions.size() - 1].pseudo_registers;
		if (pseudo_list.find(variable) != pseudo_list.end())
			return pseudo_list[variable];
		pseudo_list[variable] = pseudo;
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

	ptr<StackOperandNode> CodeGenerator::stack(int amount) {
		return std::make_shared<StackOperandNode>(amount);
	}

	ptr<SignExtendInstructionNode> CodeGenerator::sign_extend() {
		return std::make_shared<SignExtendInstructionNode>();
	}

	ptr<DivideInstructionNode> CodeGenerator::div(ptr<ASMOperandNode> operand) {
		return std::make_shared<DivideInstructionNode>(operand);
	}

	ptr<ASMCallNode> CodeGenerator::call(const Name& label, bool is_external) {
		return std::make_shared<ASMCallNode>(label, is_external);
	}

	ptr<JumpInstructionNode> CodeGenerator::jmp(const Name& label) {
		return std::make_shared<JumpInstructionNode>(label);
	}

	ptr<JumpConditionalNode> CodeGenerator::jmpc(BinaryOperation condition, const Name& label) {
		return std::make_shared<JumpConditionalNode>(condition, label);
	}

	ptr<AllocateStackNode> CodeGenerator::stack_alloc(int amount) {
		return std::make_shared<AllocateStackNode>(amount);
	}

	ptr<ASMDeallocateStackNode> CodeGenerator::stack_dealloc(int amount) {
		return std::make_shared<ASMDeallocateStackNode>(amount);
	}

	ptr<ASMPushStackNode> CodeGenerator::push(ptr<ASMOperandNode> operand) {
		return std::make_shared<ASMPushStackNode>(operand);
	}

	void CodeGenerator::generate_jump(ptr<AIRJumpInstructionNode> jump_node, ASMInstructionList& list_output) {
		instr(jmp(jump_node->label));
	}

	void CodeGenerator::generate_jump_if_zero(ptr<AIRJumpIfZeroInstructionNode> jump_node, ASMInstructionList& list_output) {
		instr(cmp(integer(0), resolve_value(jump_node->condition)));
		instr(jmpc(BinaryOperation::EQUAL, jump_node->label));
	}

	void CodeGenerator::generate_jump_if_not_zero(ptr<AIRJumpIfNotZeroInstructionNode> jump_node, ASMInstructionList& list_output) {
		instr(cmp(integer(0), resolve_value(jump_node->condition)));
		instr(jmpc(BinaryOperation::NOT_EQUAL, jump_node->label));
	}

	void CodeGenerator::generate_label(ptr<AIRLabelNode> label_node, ASMInstructionList& list_output) {
		instr(std::make_shared<ASMLabelNode>(label_node->label));
	}

	void CodeGenerator::generate_call(ptr<AIRFunctionCallNode> call_node, ASMInstructionList& list_output) {
		// System V Calling Convention sets 6 registers before allocating passed arguments to stack, Microsoft's requires 4
		uint8_t registers_to_set = (m_compile_for_windows ? 4 : 6);
		using R = Register;

		std::vector<Register> argument_registers = (m_compile_for_windows)
			? std::vector<Register>{ R::ECX, R::EDX, R::R8D, R::R9D }	// <- Microsoft ABI
			: std::vector<Register>{ R::EDI, R::ESI, R::EDX, R::ECX, R::R8D, R::R9D };	// <- System V ABI
		uint32_t args_size = call_node->value_list.size();
		uint32_t stack_argument_size = args_size - registers_to_set;
		if (stack_argument_size < 0) stack_argument_size = 0;

		// Padding to adjust stack alignment for calling convention
		int8_t stack_padding = (args_size % 2 == 0 ? 0 : 8);
		if (!stack_padding) instr(stack_alloc(stack_padding));

		uint32_t register_index = 0;

		// Pass Arguments first to registers
		for (uint8_t i = 0; i < args_size && i < registers_to_set; i++) {
			R reg = argument_registers[i];
			auto assembly_arg = resolve_value(call_node->value_list[i]);
			instr(mov(assembly_arg, std::make_shared<RegisterOperandNode>(reg)));
		}

		// Push any remaining Arguments to the stack
		for (uint8_t i = registers_to_set; i < args_size; i++) {
			auto assembly_arg = resolve_value(call_node->value_list[i]);
			if(assembly_arg->get_type() == ASMNodeType::REGISTER || assembly_arg->get_type() == ASMNodeType::INTEGER)
				instr(push(assembly_arg));
			else {
				instr(mov(assembly_arg, REGISTER(EAX)));
				instr(push(REGISTER(EAX)));
			}
		}

		instr(call(call_node->function, call_node->is_external));

		// Adjust SP (Stack Pointer)
		uint32_t bytes_to_remove = 8 * stack_argument_size + stack_padding;
		if (bytes_to_remove) instr(stack_dealloc(bytes_to_remove));

		auto assembly_dest = resolve_value(call_node->destination);
		instr(mov(REGISTER(EAX), assembly_dest));
	}

	void CodeGenerator::replace_pseudo_registers() {
		for (auto& function : m_functions) {
			// Allocate Stack Size 4 times the pseudo registers the function has (each one is 4 bytes)
			uint64_t to_allocate = function.pseudo_registers.size() * 4;
			// And round it up to nearest multiple of 16
			uint8_t remainder = to_allocate % 16;
			to_allocate += 16 - remainder;

			function.instructions->push_front(std::make_shared<AllocateStackNode>(to_allocate));
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