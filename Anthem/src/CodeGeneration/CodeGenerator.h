// CodeGenerator.h
// Contains the CodeGenerator Class definition
// Copyright (c) 2024-present, Stylianos Kementzetzidis

#pragma once
#include "Utilities/Error.h"
#include "ASMProgramStruct.h"
#include "AIR/AIR.h"

namespace Anthem {
	class CodeGenerator {
	public:
		CodeGenerator(ErrorHandler* error_handler, bool compile_for_windows);

		// Generate an ASM Program Tree from and AST
		ptr<ASMProgramNode> generate(ptr<AIRProgramNode> program, std::vector<ptr<AIRFlaggedVarNode>>& flagged_vars);
	private:
		// Create ASM Program from AST Program Node
		ptr<ASMProgramNode> generate_program(ptr<AIRProgramNode> program_node, std::vector<ptr<AIRFlaggedVarNode>>& flagged_vars);
		// Create an ASM Declaration (Function or Global Variable) Node from AST Declaration Node
		ptr<ASMDeclarationNode> generate_declaration(ptr<AIRDeclarationNode> declaration_node);

		// -- Declaration Generation --
		
		ptr<ASMFunctionNode> generate_function_declaration(ptr<AIRFunctionNode> function_node);
		ptr<ASMFlaggedVar> generate_flagged_var(ptr<AIRFlaggedVarNode> function_node);

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
		void generate_call(ptr<AIRFunctionCallNode> call_node, ASMInstructionList& list_output);

		// -- Simple Instruction Generation -- (Create a Shared Ptr)

		ptr<MoveInstructionNode> mov(ptr<ASMOperandNode> source, ptr<ASMOperandNode> destination);
		ptr<ReturnInstructionNode> ret();
		ptr<CompareInstructionNode> cmp(ptr<ASMOperandNode> source, ptr<ASMOperandNode> destination);
		ptr<SetConditionalNode> set(BinaryOperation operation, ptr<ASMOperandNode> destination);
		ptr<IntegerOperandNode> integer(int integer);
		ptr<StackOperandNode> stack(int amount);
		ptr<SignExtendInstructionNode> sign_extend();
		ptr<DivideInstructionNode> div(ptr<ASMOperandNode> operand);
		ptr<JumpInstructionNode> jmp(const Name& label);
		ptr<JumpConditionalNode> jmpc(BinaryOperation condition, const Name& label);
		ptr<AllocateStackNode> stack_alloc(int amount);
		ptr<ASMDeallocateStackNode> stack_dealloc(int amount);
		ptr<ASMPushStackNode> push(ptr<ASMOperandNode> operand);
		ptr<ASMCallNode> call(const Name& label, bool is_external = false);

		// -- Operand Resolution and Instruction Generation --
		ptr<ASMOperandNode> resolve_value(ptr<AIRValueNode> expression);

		ptr<PseudoOperandNode> make_pseudo_register(ptr<AIRVariableValueNode> variable);
		ptr<PseudoOperandNode> make_pseudo_register(const Name& variable);

		// -- Subsequent Passes --

		void replace_pseudo_registers();
		void validate_instructions(ptr<ASMProgramNode> program_node);
	private:
		struct FunctionInfo {
			ASMInstructionList* instructions{ nullptr };
			std::unordered_map<Name, ptr<PseudoOperandNode>> pseudo_registers{};
			std::unordered_map<Name, ptr<PseudoOperandNode>> flagged_vars{};
		};

		ErrorHandler* m_error_handler;
		std::vector<FunctionInfo> m_functions;
		bool m_compile_for_windows;
	};
}