#pragma once
#include "Utilities/Error.h"
#include "ASMProgramStruct.h"
#include "AIR/AIR.h"

namespace Anthem {
	class CodeGenerator {
	public:
		CodeGenerator(ErrorHandler* error_handler);

		// Generate an ASM Program Tree from and AST
		ptr<ASMProgramNode> generate(ptr<AIRProgramNode> program);
	private:
		// Create ASM Program from AST Program Node
		ptr<ASMProgramNode> generate_program(ptr<AIRProgramNode> program_node);
		// Create an ASM Declaration (Function or Global Variable) Node from AST Declaration Node
		ptr<ASMDeclarationNode> generate_declaration(ptr<AIRDeclarationNode> declaration_node);

		// -- Declaration Generation --
		
		ptr<ASMFunctionNode> generate_function_declaration(ptr<AIRFunctionNode> function_node);

		// -- ASM Instruction Generation --

		void generate_instruction(ptr<AIRInstructionNode> instruction_node, ASMInstructionList& list_output);
		void generate_return(ptr<AIRReturnInstructionNode> return_node, ASMInstructionList& list_output);
		void generate_unary(ptr<AIRUnaryInstructionNode> unary_node, ASMInstructionList& list_output);
		void generate_binary(ptr<AIRBinaryInstructionNode> binary_node, ASMInstructionList& list_output);

		// -- Interal Instructions --

		ptr<MoveInstructionNode> move_instruction(ptr<ASMOperandNode> source, ptr<ASMOperandNode> destination);
		ptr<ReturnInstructionNode> return_instruction();


		// -- Operand Resolution and Instruction Generation --

		ptr<ASMOperandNode> resolve_expression(ptr<ExpressionNode> expression);
		ptr<ASMOperandNode> resolve_value(ptr<AIRValueNode> expression);

		ptr<PseudoOperandNode> make_pseudo_register(ptr<AIRVariableValueNode> variable);

		// -- Subsequent Passes --

		void replace_pseudo_registers();
		void validate_instructions(ptr<ASMProgramNode> program_node);
	private:
		struct FunctionInfo {
			ASMInstructionList* instructions{ nullptr };
			std::unordered_map<Name, ptr<PseudoOperandNode>> pseudo_registers{};
		};

		ErrorHandler* m_error_handler;
		std::vector<FunctionInfo> m_functions;
	};
}