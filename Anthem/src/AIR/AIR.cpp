#include "AIR.h"
#include <iostream>

namespace Anthem {
	// Utility
	BinaryOperation token_to_bin_op(const Token& token) {
		switch (token.type) {
		case MINUS: return BinaryOperation::SUBTRACTION;
		case PLUS: return BinaryOperation::ADDITION;
		case STAR: return BinaryOperation::MULTIPLICATION;
		case SLASH: return BinaryOperation::DIVISION;
		case PERCENT: return BinaryOperation::REMAINDER;
		case GREATER: return BinaryOperation::GREATER;
		case GREATER_EQUAL: return BinaryOperation::GREATER_EQUAL;
		case LESS: return BinaryOperation::LESS;
		case LESS_EQUAL: return BinaryOperation::LESS_EQUAL;
		case EQUAL_EQUAL: return BinaryOperation::EQUAL;
		case BANG_EQUAL: return BinaryOperation::NOT_EQUAL;
		case AND: return BinaryOperation::AND;
		case OR: return BinaryOperation::OR;
		}
	}

	AIRGenerator::AIRGenerator(ErrorHandler* error_handler) : m_error_handler{ error_handler } {}

	Name AIRGenerator::make_temporary_name() {
		return std::format("#{0}", m_temp_counter++);
	}

	void AIRGenerator::pretty_print(ptr<AIRNode> node) {
		switch (node->get_type()) {
		case AIRNodeType::PROGRAM: {
			std::cout << "PROGRAM\n";
			for (auto& declaration : std::static_pointer_cast<AIRProgramNode>(node)->declarations)
				pretty_print(declaration);
			std::cout << '\n';
			break;
		}
		case AIRNodeType::FUNCTION: {
			ptr<AIRFunctionNode> function = std::static_pointer_cast<AIRFunctionNode>(node);
			std::cout << "FUNCTION " << function->name << ":\n";
			for (auto& instruction : function->instructions)
				pretty_print(instruction);
			std::cout << "\n\n";
			break;
		}
		case AIRNodeType::RETURN: {
			ptr<AIRReturnInstructionNode> return_instruction = std::static_pointer_cast<AIRReturnInstructionNode>(node);
			std::cout << "RETURN ";
			pretty_print(return_instruction->value);
			std::cout << '\n';
			break;
		}
		case AIRNodeType::UNARY_OPERATION: {
			ptr<AIRUnaryInstructionNode> unary_op = std::static_pointer_cast<AIRUnaryInstructionNode>(node);
			pretty_print(unary_op->destination);
			std::cout << " = ";
			switch (unary_op->operation) {
			case UnaryOperation::COMPLEMENT:
				std::cout << "COMPLEMENT ";
				break;
			case UnaryOperation::NEGATE:
				std::cout << "NEGATE ";
				break;
			}
			pretty_print(unary_op->source);
			std::cout << '\n';
			break;
		}
		case AIRNodeType::BINARY_OPERATION: {
			ptr<AIRBinaryInstructionNode> binary_op = std::static_pointer_cast<AIRBinaryInstructionNode>(node);
			pretty_print(binary_op->destination);
			std::cout << " = ";
			switch (binary_op->operation) {
			case BinaryOperation::ADDITION:
				std::cout << "ADD ";
				break;
			case BinaryOperation::SUBTRACTION:
				std::cout << "SUBTRACT ";
				break;
			case BinaryOperation::MULTIPLICATION:
				std::cout << "MULTIPLY ";
				break;
			case BinaryOperation::DIVISION:
				std::cout << "DIVIDE ";
				break;
			case BinaryOperation::REMAINDER:
				std::cout << "REMAINDER ";
				break;
			case BinaryOperation::GREATER:
				std::cout << "GREATER ";
				break;
			case BinaryOperation::GREATER_EQUAL:
				std::cout << "GREATER_EQUAL ";
				break;
			case BinaryOperation::LESS:
				std::cout << "LESS ";
				break;
			case BinaryOperation::LESS_EQUAL:
				std::cout << "LESS_EQUAL ";
				break;
			case BinaryOperation::EQUAL:
				std::cout << "EQUAL ";
				break;
			case BinaryOperation::NOT_EQUAL:
				std::cout << "NOT_EQUAL ";
				break;
			}
			pretty_print(binary_op->source_a);
			std::cout << ", ";
			pretty_print(binary_op->source_b);
			std::cout << '\n';
			break;
		}
		case AIRNodeType::JUMP: {
			ptr<AIRJumpInstructionNode> jump = std::static_pointer_cast<AIRJumpInstructionNode>(node);
			std::cout << "JUMP " << jump->label << '\n';
			break;
		}
		case AIRNodeType::JUMP_IF_ZERO: {
			ptr<AIRJumpIfZeroInstructionNode> jump_zero = std::static_pointer_cast<AIRJumpIfZeroInstructionNode>(node);
			std::cout << "JUMP_IF_ZERO ";
			pretty_print(jump_zero->condition);
			std::cout << ", " << jump_zero->label << '\n';
			break;
		}
		case AIRNodeType::JUMP_IF_NOT_ZERO: {
			ptr<AIRJumpIfNotZeroInstructionNode> jump_zero = std::static_pointer_cast<AIRJumpIfNotZeroInstructionNode>(node);
			std::cout << "JUMP_IF_NOT_ZERO ";
			pretty_print(jump_zero->condition);
			std::cout << ", " << jump_zero->label << '\n';
			break;
		}
		case AIRNodeType::SET: {
			ptr<AIRSetInstructionNode> set = std::static_pointer_cast<AIRSetInstructionNode>(node);
			pretty_print(set->variable);
			std::cout << " = ";
			pretty_print(set->value);
			std::cout << '\n';
			break;
		}
		case AIRNodeType::LABEL: {
			ptr<AIRLabelNode> label = std::static_pointer_cast<AIRLabelNode>(node);
			std::cout << "LABEL " << label->label << ":\n";
			break;
		}
		case AIRNodeType::VARIABLE: {
			ptr<AIRVariableValueNode> variable = std::static_pointer_cast<AIRVariableValueNode>(node);
			std::cout << variable->variable;
			break;
		}
		case AIRNodeType::INTEGER: {
			ptr<AIRIntegerValueNode> integer = std::static_pointer_cast<AIRIntegerValueNode>(node);
			std::cout << integer->integer;
			break;
		}
		}
	}

	ptr<AIRVariableValueNode> AIRGenerator::make_variable(Name name) {
		return std::make_shared<AIRVariableValueNode>(name);
	}

	ptr<AIRProgramNode> AIRGenerator::generate(ptr<ProgramNode> program) {
		return generate_program(program);
	}

	ptr<AIRProgramNode> AIRGenerator::generate_program(ptr<ProgramNode> program_node) {
		ptr<AIRProgramNode> AIR_program_node = std::make_shared<AIRProgramNode>();

		// Generate AIR Declarations from every one of the AST Program Node
		for (auto& declaration : program_node->declarations)
			AIR_program_node->declarations.push_back(generate_declaration(declaration));

		return AIR_program_node;
	}

	ptr<AIRDeclarationNode> AIRGenerator::generate_declaration(ptr<DeclarationNode> declaration_node) {
		// Handle any Declaration Case
		switch (declaration_node->get_type()) {
		case NodeType::FUNCTION_DECLARATION:
			return generate_function_declaration(std::static_pointer_cast<FunctionDeclarationNode>(declaration_node));
		default:
			return nullptr;
		}
	}

	ptr<AIRFunctionNode> AIRGenerator::generate_function_declaration(ptr<FunctionDeclarationNode> function_node) {
		ptr<AIRFunctionNode> AIR_function_node = std::make_shared<AIRFunctionNode>();
		AIR_function_node->name = function_node->name;
		generate_statement(function_node->body, AIR_function_node->instructions);
		return AIR_function_node;
	}

	void AIRGenerator::generate_statement(ptr<StatementNode> statement_node, AIRInstructionList& output) {
		switch (statement_node->get_type()) {
		case NodeType::RETURN_STATEMENT:
			generate_return(std::static_pointer_cast<ReturnStatementNode>(statement_node), output);
		default:
			return;
		}
	}

	void AIRGenerator::generate_return(ptr<ReturnStatementNode> return_statement_node, AIRInstructionList& output) {
		output.push_back(std::make_shared<AIRReturnInstructionNode>(resolve_expression(return_statement_node->expression, output)));
	}

	ptr<AIRValueNode> AIRGenerator::resolve_expression(ptr<ExpressionNode> expression, AIRInstructionList& output) {
		switch (expression->get_type())
		{
		case NodeType::INT_LITERAL:
			return std::make_shared<AIRIntegerValueNode>(std::static_pointer_cast<IntegerLiteralNode>(expression)->integer);
		case NodeType::UNARY_OPERATION:
			return unary_operation(std::static_pointer_cast<UnaryOperationNode>(expression), output);
		case NodeType::BINARY_OPERATION:
			return binary_operation(std::static_pointer_cast<BinaryOperationNode>(expression), output);
		default:
			return nullptr;
		}
	}

	ptr<AIRValueNode> AIRGenerator::unary_operation(ptr<UnaryOperationNode> unary_op, AIRInstructionList& output) {
		ptr<AIRValueNode> source = resolve_expression(unary_op->expression, output);
		Name destination_name = make_temporary_name();
		ptr<AIRVariableValueNode> destination = make_variable(destination_name);
		UnaryOperation operation;
		switch (unary_op->operator_token.type) {
		case MINUS:
			operation = UnaryOperation::NEGATE;
			break;
		case PLUS:
			operation = UnaryOperation::NONE;
			break;
		case TILDE:
			operation = UnaryOperation::COMPLEMENT;
			break;
		case BANG:
			operation = UnaryOperation::NOT;
			break;
		default:
			return nullptr;
		}
		output.push_back(std::make_shared<AIRUnaryInstructionNode>(operation, source, destination));
		return destination;
	}

	ptr<AIRValueNode> AIRGenerator::binary_operation(ptr<BinaryOperationNode> binary_op, AIRInstructionList& output) {
		BinaryOperation operation = token_to_bin_op(binary_op->operator_token); 
		if (operation == BinaryOperation::AND || operation == BinaryOperation::OR)
			return logical_binary_operation(binary_op, output);
		ptr<AIRValueNode> source_a = resolve_expression(binary_op->left_expression, output);
		ptr<AIRValueNode> source_b = resolve_expression(binary_op->right_expression, output);

		Name destination_name = make_temporary_name();
		ptr<AIRVariableValueNode> destination = make_variable(destination_name);

		output.push_back(std::make_shared<AIRBinaryInstructionNode>(operation, source_a, source_b, destination));
		return destination;
	}

	ptr<AIRValueNode> AIRGenerator::logical_binary_operation(ptr<BinaryOperationNode> binary_op, AIRInstructionList& output) {
		Name early_leave_label = std::format("early_leave.{0}", m_global_label_counter);
		Name end_label = std::format("end.{0}", m_global_label_counter);
		// Set true to evaluate AND operation, false to evaluate OR operation
		bool and_operation = true;

		/*		if (binary_op->operator_token.type == AND) and_operation = true;	*/			// Uneeded
		if (binary_op->operator_token.type == OR) and_operation = false;

		BinaryOperation operation = BinaryOperation::AND;
		// Resolve left expression
		ptr<AIRValueNode> source_a = resolve_expression(binary_op->left_expression, output);
		// If left expression is false (or true depending on the operation), short-circuit the operation, dont resolve right expression and jump to false/true label
		if(and_operation)
			output.push_back(std::make_shared<AIRJumpIfZeroInstructionNode>(source_a, early_leave_label));
		else
			output.push_back(std::make_shared<AIRJumpIfNotZeroInstructionNode>(source_a, early_leave_label));

		// Resolve right expression
		ptr<AIRValueNode> source_b = resolve_expression(binary_op->right_expression, output);
		// If right expression is also false (or true depending on the operation) jump to false/true label
		if (and_operation)
			output.push_back(std::make_shared<AIRJumpIfZeroInstructionNode>(source_b, early_leave_label));
		else
			output.push_back(std::make_shared<AIRJumpIfNotZeroInstructionNode>(source_b, early_leave_label));

		// Return variable
		Name destination_name = make_temporary_name();
		ptr<AIRVariableValueNode> destination = make_variable(destination_name);

		// If both sides evaluated to true (or false), set the return value to true (or false) and skip over the early_leave label instruction
		if (and_operation)
			output.push_back(std::make_shared<AIRSetInstructionNode>(destination, std::make_shared<AIRIntegerValueNode>(1)));
		else
			output.push_back(std::make_shared<AIRSetInstructionNode>(destination, std::make_shared<AIRIntegerValueNode>(0)));
		output.push_back(std::make_shared<AIRJumpInstructionNode>(end_label));

		// False label, set return value to false
		output.push_back(std::make_shared<AIRLabelNode>(early_leave_label));
		if(and_operation)
			output.push_back(std::make_shared<AIRSetInstructionNode>(destination, std::make_shared<AIRIntegerValueNode>(0)));
		else
			output.push_back(std::make_shared<AIRSetInstructionNode>(destination, std::make_shared<AIRIntegerValueNode>(1)));

		output.push_back(std::make_shared<AIRLabelNode>(end_label));

		// Increment label counter in order to have unique identifiers
		m_global_label_counter++;
		return destination;
	}
}