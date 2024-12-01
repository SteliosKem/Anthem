#include "CodeGenerator.h"

namespace Anthem {
	CodeGenerator::CodeGenerator(ErrorHandler* error_handler) : m_error_handler{ error_handler } {}

	ptr<ASMProgramNode> CodeGenerator::generate(ptr<ProgramNode> program) {
		return generate_program(program);
	}

	ptr<ASMProgramNode> CodeGenerator::generate_program(ptr<ProgramNode> program_node) {
		ptr<ASMProgramNode> asm_program_node = std::make_shared<ASMProgramNode>();
		
		// Generate ASM Declarations from every one of the AST Program Node
		for (auto& declaration : program_node->declarations)
			asm_program_node->declarations.push_back(generate_declaration(declaration));

		return asm_program_node;
	}

	ptr<ASMDeclarationNode> CodeGenerator::generate_declaration(ptr<DeclarationNode> declaration_node) {
		// Handle any Declaration Case
		switch (declaration_node->get_type()) {
		case NodeType::FUNCTION_DECLARATION:
			return generate_function_declaration(std::static_pointer_cast<FunctionDeclarationNode>(declaration_node));
		default:
			return nullptr;
		}
	}

	ptr<ASMFunctionNode> CodeGenerator::generate_function_declaration(ptr<FunctionDeclarationNode> function_node) {
		ptr<ASMFunctionNode> asm_function_node = std::make_shared<ASMFunctionNode>();
		asm_function_node->name = function_node->name;
		asm_function_node->instruction = generate_statement(function_node->body);
		return asm_function_node;
	}

	ptr<ASMInstructionNode> CodeGenerator::generate_statement(ptr<StatementNode> statement_node) {
		switch (statement_node->get_type()) {
		case NodeType::RETURN_STATEMENT:
			return generate_return(std::static_pointer_cast<ReturnStatementNode>(statement_node));
		default:
			return nullptr;
		}
	}

	ptr<ASMInstructionNode> CodeGenerator::generate_return(ptr<ReturnStatementNode> return_statement_node) {
		ptr<InstructionPackNode> instruction_pack = std::make_shared<InstructionPackNode>();
		// Move the result of the return expression to EAX register
		instruction_pack->instruction_list.push_back(move_instruction(resolve_expression(return_statement_node->expression), REGISTER(EAX)));
		// Return
		instruction_pack->instruction_list.push_back(return_instruction());
		return instruction_pack;
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
}