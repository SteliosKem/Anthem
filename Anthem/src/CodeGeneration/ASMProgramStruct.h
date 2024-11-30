#pragma once
#include <string>
#include <vector>
#include "Utilities/Utilities.h"

namespace Anthem {
	// Macro to simplify basic setup for all node classes
#define ASM_NODE_TYPE(x) ASMNodeType get_type() const override { return ASMNodeType::x; }

	using Name = std::string;

	enum Register {
		EAX
	};

	enum class ASMNodeType {
		INSTRUCTION,
		OPERAND,
		PROGRAM,
		DECLARATION,

		FUNCTION,

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
		ASMFunctionNode(const Name& name, const ASMInstructionList& instructions)
			: name{ name }, instructions{ instructions } {}

		ASM_NODE_TYPE(FUNCTION)
	public:
		Name name;
		ASMInstructionList instructions;
	};

	// Operand Nodes

	class INTOperandNode : public ASMOperandNode {
	public:
		INTOperandNode(int number) : integer{ number } {}

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

	// Instruction Nodes

	class ReturnInstructionNode : public ASMInstructionNode {
	public:
		ASM_NODE_TYPE(RETURN)
	};

	class MoveInstructionNode : public ASMInstructionNode {
	public:
		MoveInstructionNode(ptr<ASMOperandNode> source, ptr<ASMOperandNode> destination)
			: source{ source }, destination{ destination } {}

		ASM_NODE_TYPE(REGISTER)
	public:
		ptr<ASMOperandNode> source;
		ptr<ASMOperandNode> destination;
	};
}