#pragma once
#include "Utilities/Error.h"
#include "ASMProgramStruct.h"
#include "Parser/Parser.h"

namespace Anthem {
	class CodeGenerator {
	public:
		CodeGenerator(ErrorHandler* error_handler);

		// Generate an ASM Program Tree from and AST
		ptr<ASMProgramNode> generate(ptr<ProgramNode> program);
	private:
		// Create ASM Program from AST Program Node
		ptr<ASMProgramNode> generate_program(ptr<ProgramNode> program_node);
		// Create an ASM Declaration (Function or Global Variable) Node from AST Declaration Node
		ptr<ASMDeclarationNode> generate_declaration(ptr<DeclarationNode> declaration_node);

		// -- Declaration Generation --
		
		ptr<ASMFunctionNode> generate_function_declaration(ptr<FunctionDeclarationNode> function_node);


		// -- Statement Generation --

		ptr<ASMInstructionNode> generate_statement(ptr<StatementNode> statement_node);
		ptr<ASMInstructionNode> generate_return(ptr<ReturnStatementNode> return_node);


		// -- ASM Instruction Generation --

		ptr<MoveInstructionNode> move_instruction(ptr<ASMOperandNode> source, ptr<ASMOperandNode> destination);
		ptr<ReturnInstructionNode> return_instruction();


		// -- Operand Resolution and Instruction Generation --

		ptr<ASMOperandNode> resolve_expression(ptr<ExpressionNode> expression);
	private:
		ErrorHandler* m_error_handler;
	};
}