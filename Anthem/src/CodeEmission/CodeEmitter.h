#pragma once
#include "CodeGeneration/CodeGenerator.h"

namespace Anthem {
	enum class Size {
		BYTE,
		WORD,
		DWORD,
		QWORD
	};

	/* Base Class that defines general emitting behaviour.
	*  Derived classes implement the pure virtual functions, in order to have the ability
	*  to implement assembly emission for more architectures / syntaxes
	*/
	class CodeEmitter {
	public:
		CodeEmitter() = default;

		virtual void emit(ptr<ASMProgramNode> program_node, std::string& assembly_out);
	protected:
		virtual void emit_program(ptr<ASMProgramNode> program);
		virtual void emit_global_identifier(const std::string& identifier) = 0;
		virtual void emit_function(ptr<ASMFunctionNode> function);
		virtual void emit_declaration(ptr<ASMDeclarationNode> declaration);
		virtual void emit_label(const std::string& name) = 0;
		virtual void emit_function_prologue() = 0;
		virtual void emit_function_epilogue() = 0;
		virtual void emit_instruction(ptr<ASMInstructionNode> instruction);
		virtual void emit_move(ptr<MoveInstructionNode> move_instruction) = 0;
		virtual void emit_operand(ptr<ASMOperandNode> operand, Size size = Size::DWORD);
		virtual void emit_register(Register register_op, Size size = Size::DWORD) = 0;
		virtual void emit_integer(int integer) = 0;
		virtual void emit_return() = 0;
		virtual void emit_file_epilogue() = 0;
		virtual void emit_unary(ptr<UnaryInstructionNode> unary_operation) = 0;
		virtual void emit_binary(ptr<BinaryInstructionNode> binary_operation) = 0;
		virtual void emit_compare(ptr<CompareInstructionNode> compare_operation) = 0;
		virtual void emit_jump(ptr<JumpInstructionNode> jump) = 0;
		virtual void emit_jump_conditional(ptr<JumpConditionalNode> conditional_jump) = 0;
		virtual void emit_set_conditional(ptr<SetConditionalNode> set_conditional) = 0;
		virtual void emit_idiv(ptr<DivideInstructionNode> divide) = 0;
		virtual void emit_cdq() = 0;
		virtual void emit_stack_access(int offset) = 0;
		virtual void emit_allocate_stack(ptr<AllocateStackNode> stack_operand) = 0;
		virtual void emit_deallocate_stack(ptr<ASMDeallocateStackNode> stack_operand) = 0;
		virtual void emit_push_stack(ptr<ASMPushStackNode> operand) = 0;
		virtual void emit_call(ptr<ASMCallNode> call) = 0;
		virtual const std::string& assembly_out() = 0;
	};
}