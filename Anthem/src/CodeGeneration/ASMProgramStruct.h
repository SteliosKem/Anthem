// AIRConstructs.h
// Contains Node definitions for ASM trees
// Copyright (c) 2024-present, Stylianos Kementzetzidis

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
		EDX,
		R10D,
		R11D,
		ECX,
		EDI,
		ESI,
		R8D,
		R9D
	};

	enum class ASMNodeType {
		INSTRUCTION,
		OPERAND,
		PROGRAM,
		DECLARATION,
		LABEL,

		FUNCTION,
		FLAGGED_VAR,

		// -- Operands --

		INTEGER,
		REGISTER,
		PSEUDO_OPERAND,
		STACK_OPERAND,

		// -- Instructions --

		RETURN,
		MOVE,
		UNARY,
		BINARY,
		DIVIDE,
		SIGN_EXTEND,
		ALLOCATE_STACK,
		JUMP,
		JUMP_CONDITIONAL,
		SET_CONDITIONAL,
		COMPARE,
		DEALLOCATE_STACK,
		PUSH,
		CALL,
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

	// -- Declaration Nodes --

	class ASMFunctionNode : public ASMDeclarationNode {
	public:
		ASMFunctionNode() = default;
		ASMFunctionNode(const Name& name, VarFlag flag, const ASMInstructionList& instructions)
			: name{ name }, instructions{ instructions } {}

		ASM_NODE_TYPE(FUNCTION)
	public:
		Name name;
		VarFlag flag;
		ASMInstructionList instructions;
	};

	class ASMFlaggedVar : public ASMDeclarationNode {
	public:
		ASMFlaggedVar() = default;
		ASMFlaggedVar(const Name& name, VarFlag flag, int initializer)
			: name{ name }, flag{ flag }, initializer{ initializer } {}

		ASM_NODE_TYPE(FLAGGED_VAR)
	public:
		Name name;
		VarFlag flag;
		int initializer;
	};

	// -- Operand Nodes --

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
		bool flagged = false;
	};

	// -- Instruction Nodes --

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

	class BinaryInstructionNode : public ASMInstructionNode {
	public:
		BinaryInstructionNode(BinaryOperation binary_operation, ptr<ASMOperandNode> operand_a, ptr<ASMOperandNode> operand_b)
			: binary_operation{ binary_operation }, operand_a{ operand_a }, operand_b{ operand_b } {}

		ASM_NODE_TYPE(BINARY)
	public:
		BinaryOperation binary_operation;
		ptr<ASMOperandNode> operand_a;
		ptr<ASMOperandNode> operand_b;
	};

	class DivideInstructionNode : public ASMInstructionNode {
	public:
		DivideInstructionNode() = default;
		DivideInstructionNode(ptr<ASMOperandNode> operand) : operand{ operand } {}

		ASM_NODE_TYPE(DIVIDE)
	public:
		ptr<ASMOperandNode> operand;
	};

	class SignExtendInstructionNode : public ASMInstructionNode {
	public:
		ASM_NODE_TYPE(SIGN_EXTEND)
	};

	class CompareInstructionNode : public ASMInstructionNode {
	public:
		CompareInstructionNode() = default;
		CompareInstructionNode(ptr<ASMOperandNode> operand_a, ptr<ASMOperandNode> operand_b)
			: operand_a{ operand_a }, operand_b{ operand_b } {}

		ASM_NODE_TYPE(COMPARE)
	public:
		ptr<ASMOperandNode> operand_a;
		ptr<ASMOperandNode> operand_b;
	};

	class JumpInstructionNode : public ASMInstructionNode {
	public:
		JumpInstructionNode() = default;
		JumpInstructionNode(const Name& label) : label{ label } {}

		ASM_NODE_TYPE(JUMP)
	public:
		Name label;
	};

	class JumpConditionalNode : public ASMInstructionNode {
	public:
		JumpConditionalNode() = default;
		JumpConditionalNode(BinaryOperation condition, const Name& label) : label{ label }, condition{ condition } {}

		ASM_NODE_TYPE(JUMP_CONDITIONAL)
	public:
		Name label;
		BinaryOperation condition;
	};

	class SetConditionalNode : public ASMInstructionNode {
	public:
		SetConditionalNode() = default;
		SetConditionalNode(BinaryOperation condition, ptr<ASMOperandNode> operand) : condition{ condition }, operand{ operand } {}

		ASM_NODE_TYPE(SET_CONDITIONAL)
	public:
		BinaryOperation condition;
		ptr<ASMOperandNode> operand;
	};

	class ASMLabelNode : public ASMInstructionNode {
	public:
		ASMLabelNode() = default;
		ASMLabelNode(const Name& label) : label{ label } {}

		ASM_NODE_TYPE(LABEL)
	public:
		Name label;
	};

	class ASMDeallocateStackNode : public ASMInstructionNode {
	public:
		ASMDeallocateStackNode() = default;
		ASMDeallocateStackNode(int amount) : amount{ amount } {}

		ASM_NODE_TYPE(DEALLOCATE_STACK)
	public:
		int amount;
	};

	class ASMPushStackNode : public ASMInstructionNode {
	public:
		ASMPushStackNode() = default;
		ASMPushStackNode(ptr<ASMOperandNode> operand) : operand{ operand } {}

		ASM_NODE_TYPE(PUSH)
	public:
		ptr<ASMOperandNode> operand;
	};

	class ASMCallNode : public ASMInstructionNode {
	public:
		ASMCallNode() = default;
		ASMCallNode(const Name& label, bool is_external) : label{ label }, is_external{ is_external } {}

		ASM_NODE_TYPE(CALL)
	public:
		Name label;
		bool is_external;
	};
}