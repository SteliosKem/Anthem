#pragma once
#include "CodeEmitter.h"

namespace Anthem {
	class x86_GAS_Emitter : public CodeEmitter {
	public:
		x86_GAS_Emitter() = default;
	private:
		virtual void emit_global_identifier(const std::string& identifier) override;
		virtual void emit_label(const std::string& name) override;
		virtual void emit_function_prologue() override;
		virtual void emit_function_epilogue() override;
		virtual void emit_move(ptr<MoveInstructionNode> move_instruction) override;
		virtual void emit_operand(ptr<ASMOperandNode> operand) override;
		virtual void emit_register(Register register_op) override;
		virtual void emit_integer(int integer) override;
		virtual void emit_return() override;
		virtual void emit_file_epilogue() override;

		virtual const std::string& assembly_out() override;

		void emit_string(const std::string& str, bool idented = true);
		void emit_line();
	private:
		std::string m_assembly_output;
	};
}