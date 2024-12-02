#pragma once
#include <string>
#include <vector>
#include "Utilities/Utilities.h"
#include <deque>

namespace Anthem {
// Macro to simplify basic setup for all node classes
#define ASM_NODE_TYPE(x) ASMNodeType get_type() const override { return ASMNodeType::x; }

	using Name = std::string;

	enum class Register {
		EAX,
		R10D,
	};

	enum class ASMNodeType {
		INSTRUCTION,
		OPERAND,
		PROGRAM,
		DECLARATION,

		FUNCTION,

		// -- Operands --

		INTEGER,
		REGISTER,
		PSEUDO_OPERAND,
		STACK_OPERAND,

		// -- Instructions --

		RETURN,
		MOVE,
		UNARY,
		ALLOCATE_STACK,
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
	using ASMInstructionList = std::deque<ptr<ASMInstructionNode>>;

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
		ASMFunctionNode(const Name& name, const ASMInstructionList& instructions)
			: name{ name }, instructions{ instructions } {}

		ASM_NODE_TYPE(FUNCTION)
	public:
		Name name;
		ASMInstructionList instructions;
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

	class UnaryInstructionNode : public ASMInstructionNode {
	public:
		UnaryInstructionNode(UnaryOperation unary_operation, ptr<ASMOperandNode> operand)
			: unary_operation{ unary_operation }, operand{ operand } {}

		ASM_NODE_TYPE(UNARY)
	public:
		UnaryOperation unary_operation;
		ptr<ASMOperandNode> operand;
	};

	class AllocateStackNode : public ASMInstructionNode {
	public:
		AllocateStackNode(int position) : position{ position } {}

		ASM_NODE_TYPE(ALLOCATE_STACK)
	public:
		int position;
	};

	class StackOperandNode : public ASMOperandNode {
	public:
		StackOperandNode(int position) : position{ position } {}

		ASM_NODE_TYPE(STACK_OPERAND)
	public:
		int position;
	};

	// This also acts as a StackOperandNode
	class PseudoOperandNode : public ASMOperandNode {
	public:
		PseudoOperandNode(Name name) : name{ name } {}

		ASM_NODE_TYPE(PSEUDO_OPERAND)
	public:
		Name name;
		int stack_offset{ 0 };
	};
}