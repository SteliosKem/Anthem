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

		INSTRUCTION_PACK,
		UNARY_OPERATION,
		INTEGER,
		VARIABLE,
		RETURN,
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

	class AIRReturnInstructionNode : public AIRInstructionNode {
	public:
		AIRReturnInstructionNode(ptr<AIRValueNode> value) : value{ value } {}

		AIR_NODE_TYPE(RETURN)
	public:
		ptr<AIRValueNode> value;
	};
}