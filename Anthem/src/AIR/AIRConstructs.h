#pragma once
#include <string>
#include "Utilities/Utilities.h"

namespace Anthem {
	// Macro to simplify basic setup for all node classes
#define AIR_NODE_TYPE(x) AIRNodeType get_type() const override { return AIRNodeType::x; }

	using Name = std::string;

	enum class AIRNodeType {
		INSTRUCTION,
		VALUE,
		PROGRAM,
		DECLARATION,

		FUNCTION,

		UNARY_OPERATION,
		BINARY_OPERATION,
		JUMP,
		JUMP_IF_ZERO,
		JUMP_IF_NOT_ZERO,
		SET,
		LABEL,
		INTEGER,
		VARIABLE,
		RETURN,
		CALL
	};

	// General Nodes

	class AIRNode {
	public:
		virtual AIRNodeType get_type() const = 0;
	};


	class AIRDeclarationNode : public AIRNode {
	public:
		AIR_NODE_TYPE(DECLARATION)
	};

	class AIRInstructionNode : public AIRNode {
	public:
		AIR_NODE_TYPE(INSTRUCTION)
	};

	class AIRValueNode : public AIRNode {
	public:
		AIR_NODE_TYPE(VALUE)
	};

	using AIRDeclarationList = std::vector<ptr<AIRDeclarationNode>>;
	using AIRInstructionList = std::vector<ptr<AIRInstructionNode>>;

	class AIRProgramNode : public AIRNode {
	public:
		AIRProgramNode() = default;
		AIRProgramNode(const AIRDeclarationList& list) : declarations{ list } {}

		AIR_NODE_TYPE(PROGRAM)
	public:
		AIRDeclarationList declarations;
	};

	// Declaration Nodes

	class AIRFunctionNode : public AIRDeclarationNode {
	public:
		AIRFunctionNode() = default;
		AIRFunctionNode(const Name& name, const AIRInstructionList& instructions)
			: name{ name }, instructions{ instructions } {}

		AIR_NODE_TYPE(FUNCTION)
	public:
		Name name;
		std::vector<Name> parameters;
		AIRInstructionList instructions;
	};

	// Operand Nodes

	class AIRIntegerValueNode : public AIRValueNode {
	public:
		AIRIntegerValueNode(int number) : integer{ number } {}

		AIR_NODE_TYPE(INTEGER)
	public:
		int integer;
	};

	class AIRVariableValueNode : public AIRValueNode {
	public:
		AIRVariableValueNode(const Name& variable) : variable{ variable } {}

		AIR_NODE_TYPE(VARIABLE)
	public:
		Name variable;
	};

	// Instruction Nodes

	class AIRUnaryInstructionNode : public AIRInstructionNode {
	public:
		AIRUnaryInstructionNode(UnaryOperation operation, ptr<AIRValueNode> source, ptr<AIRVariableValueNode> destination)
			: operation{ operation }, source{ source }, destination{ destination } {}

		AIR_NODE_TYPE(UNARY_OPERATION)
	public:
		UnaryOperation operation;
		ptr<AIRValueNode> source;
		ptr<AIRVariableValueNode> destination;
	};

	class AIRBinaryInstructionNode : public AIRInstructionNode {
	public:
		AIRBinaryInstructionNode(BinaryOperation operation, ptr<AIRValueNode> source_a, ptr<AIRValueNode> source_b, ptr<AIRVariableValueNode> destination)
			: operation{ operation }, source_a{ source_a }, source_b{ source_b }, destination{destination} {}

		AIR_NODE_TYPE(BINARY_OPERATION)
	public:
		BinaryOperation operation;
		ptr<AIRValueNode> source_a;
		ptr<AIRValueNode> source_b;
		ptr<AIRVariableValueNode> destination;
	};

	class AIRReturnInstructionNode : public AIRInstructionNode {
	public:
		AIRReturnInstructionNode(ptr<AIRValueNode> value) : value{ value } {}

		AIR_NODE_TYPE(RETURN)
	public:
		ptr<AIRValueNode> value;
	};

	class AIRJumpInstructionNode : public AIRInstructionNode {
	public:
		AIRJumpInstructionNode(const Name& label) : label{ label } {}

		AIR_NODE_TYPE(JUMP)
	public:
		Name label;
	};

	class AIRJumpIfZeroInstructionNode : public AIRInstructionNode {
	public:
		AIRJumpIfZeroInstructionNode(ptr<AIRValueNode> condition, const Name& label)
			: label{ label }, condition{ condition } {}

		AIR_NODE_TYPE(JUMP_IF_ZERO)
	public:
		Name label;
		ptr<AIRValueNode> condition;
	};

	class AIRJumpIfNotZeroInstructionNode : public AIRInstructionNode {
	public:
		AIRJumpIfNotZeroInstructionNode(ptr<AIRValueNode> condition, const Name& label)
			: label{ label }, condition{ condition } {}

		AIR_NODE_TYPE(JUMP_IF_NOT_ZERO)
	public:
		Name label;
		ptr<AIRValueNode> condition;
	};

	class AIRLabelNode : public AIRInstructionNode {
	public:
		AIRLabelNode(const Name& label) : label{ label } {}

		AIR_NODE_TYPE(LABEL)
	public:
		Name label;
	};

	class AIRSetInstructionNode : public AIRInstructionNode {
	public:
		AIRSetInstructionNode(ptr<AIRVariableValueNode> variable, ptr<AIRValueNode> value)
			: variable{ variable }, value{ value } {}

		AIR_NODE_TYPE(SET)
	public:
		ptr<AIRVariableValueNode> variable;
		ptr<AIRValueNode> value;
	};

	using ValueList = std::vector<ptr<AIRVariableValueNode>>;

	class AIRFunctionCallNode : public AIRInstructionNode {
	public:
		AIRFunctionCallNode(Name function, const ValueList& value_list, ptr<AIRValueNode> destination)
			: function{ function }, value_list{ value_list }, destination{ destination } {}

		AIR_NODE_TYPE(CALL)
	public:
		Name function;
		ValueList value_list;
		ptr<AIRValueNode> destination;
	};
}