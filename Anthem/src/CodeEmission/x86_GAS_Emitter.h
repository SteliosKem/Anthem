// x86_GAS_Emitter.h
// Contains the x86 AT&T Assembly Emmiter for Linux Class defintion
// Copyright (c) 2024-present, Stylianos Kementzetzidis

#pragma once
#include "CodeEmitter.h"

namespace Anthem {
	class x86_GAS_Emitter : public CodeEmitter {
	public:
		x86_GAS_Emitter() = default;
	protected:
		virtual void emit_global_identifier(const std::string& identifier) override;
		virtual void emit_label(const std::string& name) override;
		virtual void emit_function_prologue() override;
		virtual void emit_function_epilogue() override;
		virtual void emit_move(ptr<MoveInstructionNode> move_instruction) override;
		virtual void emit_register(Register register_op, Size size = Size::DWORD) override;
		virtual void emit_integer(int integer) override;
		virtual void emit_return() override;
		virtual void emit_file_epilogue() override;
		virtual void emit_unary(ptr<UnaryInstructionNode> unary_operation) override;
		virtual void emit_stack_access(int offset) override;
		virtual void emit_allocate_stack(ptr<AllocateStackNode> allocate_stack) override;
		virtual void emit_binary(ptr<BinaryInstructionNode> binary_operation) override;
		virtual void emit_idiv(ptr<DivideInstructionNode> divide) override;
		virtual void emit_compare(ptr<CompareInstructionNode> compare_operation) override;
		virtual void emit_jump(ptr<JumpInstructionNode> jump) override;
		virtual void emit_jump_conditional(ptr<JumpConditionalNode> conditional_jump) override;
		virtual void emit_set_conditional(ptr<SetConditionalNode> set_conditional) override;
		virtual void emit_cdq() override;
		virtual void emit_deallocate_stack(ptr<ASMDeallocateStackNode> stack_operand) override;
		virtual void emit_push_stack(ptr<ASMPushStackNode> operand) override;
		virtual void emit_call(ptr<ASMCallNode> call) override;

		virtual const std::string& assembly_out() override;

		void emit_string(const std::string& str, bool idented = true);
		void emit_line();
	private:
		std::string m_assembly_output;
	};
}