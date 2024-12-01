#pragma once
#include <string>
#include <vector>
#include "Utilities/Utilities.h"

namespace Anthem {
// Macro to simplify basic setup for all node classes
#define ASM_NODE_TYPE(x) ASMNodeType get_type() const override { return ASMNodeType::x; }

	using Name = std::string;

	enum class Register {
		EAX
	};

	enum class ASMNodeType {
		INSTRUCTION,
		OPERAND,
		PROGRAM,
		DECLARATION,

		FUNCTION,

		INSTRUCTION_PACK,
		INTEGER,
		REGISTER,
		RETURN,
		MOVE
	};

	// General Nodes

	class ASMNode {
	public:
		virtual ASMNodeType get_type() const = 0;
	};

	
	class ASMDeclarationNode : public ASMNode {
	public:
		ASM_NODE_TYPE(DECLARATION)
	};

	class ASMInstructionNode : public ASMNode {
	public:
		ASM_NODE_TYPE(INSTRUCTION)
	};

	class ASMOperandNode : public ASMNode {
	public:
		ASM_NODE_TYPE(OPERAND)
	};

	using ASMDeclarationList = std::vector<ptr<ASMDeclarationNode>>;
	using ASMInstructionList = std::vector<ptr<ASMInstructionNode>>;

	class ASMProgramNode : public ASMNode {
	public:
		ASMProgramNode() = default;
		ASMProgramNode(const ASMDeclarationList& list) : declarations{ list } {}

		ASM_NODE_TYPE(PROGRAM)
	public:
		ASMDeclarationList declarations;
	};

	// Declaration Nodes

	class ASMFunctionNode : public ASMDeclarationNode {
	public:
		ASMFunctionNode() = default;
		ASMFunctionNode(const Name& name, ptr<ASMInstructionNode> instruction)
			: name{ name }, instruction{ instruction } {}

		ASM_NODE_TYPE(FUNCTION)
	public:
		Name name;
		ptr<ASMInstructionNode> instruction;
	};

	// Operand Nodes

	class IntegerOperandNode : public ASMOperandNode {
	public:
		IntegerOperandNode(int number) : integer{ number } {}

		ASM_NODE_TYPE(INTEGER)
	public:
		int integer;
	};

	class RegisterOperandNode : public ASMOperandNode {
	public:
		RegisterOperandNode(Register register_op) : register_op{ register_op } {}

		ASM_NODE_TYPE(REGISTER)
	public:
		Register register_op;
	};
	// Macro to simplify setting a register as an operand
#define REGISTER(x) std::make_shared<RegisterOperandNode>(Register::x)

	// Instruction Nodes

	class InstructionPackNode : public ASMInstructionNode {
	public:
		InstructionPackNode() = default;
		InstructionPackNode(const ASMInstructionList& list) : instruction_list{ list } {}
		ASM_NODE_TYPE(INSTRUCTION_PACK)
	public:
		ASMInstructionList instruction_list;
	};

	class ReturnInstructionNode : public ASMInstructionNode {
	public:
		ASM_NODE_TYPE(RETURN)
	};

	class MoveInstructionNode : public ASMInstructionNode {
	public:
		MoveInstructionNode(ptr<ASMOperandNode> source, ptr<ASMOperandNode> destination)
			: source{ source }, destination{ destination } {}

		ASM_NODE_TYPE(MOVE)
	public:
		ptr<ASMOperandNode> source;
		ptr<ASMOperandNode> destination;
	};
}