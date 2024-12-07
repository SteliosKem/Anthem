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
		void handle_complex_binary(ptr<AIRBinaryInstructionNode> binary_node, ASMInstructionList& list_output);
		void generate_jump(ptr<AIRJumpInstructionNode> jump_node, ASMInstructionList& list_output);
		void generate_jump_if_zero(ptr<AIRJumpIfZeroInstructionNode> jump_node, ASMInstructionList& list_output);
		void generate_jump_if_not_zero(ptr<AIRJumpIfNotZeroInstructionNode> jump_node, ASMInstructionList& list_output);
		void generate_label(ptr<AIRLabelNode> label_node, ASMInstructionList& list_output);

		// -- Simple Instruction Generation -- (Create a Shared Ptr)

		ptr<MoveInstructionNode> mov(ptr<ASMOperandNode> source, ptr<ASMOperandNode> destination);
		ptr<ReturnInstructionNode> ret();
		ptr<CompareInstructionNode> cmp(ptr<ASMOperandNode> source, ptr<ASMOperandNode> destination);
		ptr<SetConditionalNode> set(BinaryOperation operation, ptr<ASMOperandNode> destination);
		ptr<IntegerOperandNode> integer(int integer);
		ptr<SignExtendInstructionNode> sign_extend();
		ptr<DivideInstructionNode> div(ptr<ASMOperandNode> operand);

		// -- Operand Resolution and Instruction Generation --
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