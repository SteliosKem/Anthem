#pragma once
#include "Parser/Parser.h"
#include "AIRConstructs.h"

namespace Anthem {
	class AIRGenerator {
	public:
		AIRGenerator(ErrorHandler* error_handler);

		// Generate an ASM Program Tree from and AST
		ptr<AIRProgramNode> generate(ptr<ProgramNode> program);

		static void pretty_print(ptr<AIRNode> program_node);
	private:
		Name make_temporary_name();
		ptr<AIRVariableValueNode> make_variable(Name name);

		// Create ASM Program from AST Program Node
		ptr<AIRProgramNode> generate_program(ptr<ProgramNode> program_node);
		// Create an ASM Declaration (Function or Global Variable) Node from AST Declaration Node
		ptr<AIRDeclarationNode> generate_declaration(ptr<DeclarationNode> declaration_node, AIRInstructionList* output_optional = nullptr);

		// -- Declaration Generation --

		ptr<AIRFunctionNode> generate_function_declaration(ptr<FunctionDeclarationNode> function_node);

		// -- Statement Generation --

		void generate_statement(ptr<StatementNode> statement_node, AIRInstructionList& output);
		void generate_return(ptr<ReturnStatementNode> return_node, AIRInstructionList& output);
		void generate_block(ptr<BlockStatementNode> block_statement, AIRInstructionList& output);
		void generate_if(ptr<IfStatementNode> block_statement, AIRInstructionList& output);

		// -- Expression Resolution and Instruction Generation --

		ptr<AIRValueNode> resolve_expression(ptr<ExpressionNode> expression, AIRInstructionList& output);
		ptr<AIRValueNode> unary_operation(ptr<UnaryOperationNode> unary_op, AIRInstructionList& output);
		ptr<AIRValueNode> binary_operation(ptr<BinaryOperationNode> binary_op, AIRInstructionList& output);
		ptr<AIRValueNode> assignment(ptr<AssignmentNode> binary_op, AIRInstructionList& output);

		ptr<AIRValueNode> logical_binary_operation(ptr<BinaryOperationNode> binary_op, AIRInstructionList& output);

		// -- AIR Instruction Creation --
		ptr<AIRIntegerValueNode> integer(int integer);
		ptr<AIRSetInstructionNode> set(ptr<AIRVariableValueNode> variable, ptr<AIRValueNode> value);
		ptr<AIRLabelNode> label(const Name& name);
		ptr<AIRJumpInstructionNode> jump(const Name& label);
		ptr<AIRJumpIfNotZeroInstructionNode> jump_not_zero(ptr<AIRValueNode> condition, const Name& label);
		ptr<AIRJumpIfZeroInstructionNode> jump_zero(ptr<AIRValueNode> condition, const Name& label);
	private:
		uint32_t m_global_label_counter = 0;
		ErrorHandler* m_error_handler;
		int m_temp_counter = 0;
	};
}