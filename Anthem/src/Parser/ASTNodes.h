#pragma once
#include "Utilities/Utilities.h"
#include "Lexer/Token.h"

namespace Anthem {
// Macro to simplify basic setup for all node classes
#define NODE_TYPE(x) NodeType get_type() const override { return NodeType::x; }

	// Using 'Name' type in case it gets changed from std::string
	using Name = std::string;


	enum class NodeType {
		// Groups
			DECLARATION,
			STATEMENT,
			EXPRESSION,

		PROGRAM,
		FUNCTION_DECLARATION,
		
		// Expressions
			UNARY_OPERATION,
			BINARY_OPERATION,
			INT_LITERAL,

		// Statements
			GROUP_STATEMENT,
			RETURN_STATEMENT,
	};

	// General Nodes

	class ASTNode {
	public:
		virtual NodeType get_type() const = 0;
	};

	class ExpressionNode : public ASTNode {
	public:
		NODE_TYPE(EXPRESSION)
	};

	class DeclarationNode : public ASTNode {
	public:
		NODE_TYPE(DECLARATION)
	};

	using DeclarationList = std::vector<ptr<DeclarationNode>>;

	class ProgramNode : public ASTNode {
	public:
		ProgramNode() = default;
		ProgramNode(const DeclarationList& list) : declarations{ list } {}

		NODE_TYPE(PROGRAM)
	public:
		DeclarationList declarations;
	};

	class StatementNode : public ASTNode {
	public:
		NODE_TYPE(STATEMENT)
	};

	// Declaration Nodes

	class FunctionDeclarationNode : public DeclarationNode {
	public:
		FunctionDeclarationNode(const Name& name, ptr<StatementNode> body)
			: name{ name }, body{ body } {}

		NODE_TYPE(FUNCTION_DECLARATION)
	public:
		Name name;
		ptr<StatementNode> body;
	};

	// Expression Nodes

	class UnaryOperationNode : public ExpressionNode {
	public:
		UnaryOperationNode(Token operator_token, ptr<ExpressionNode> expression) 
			: operator_token{ operator_token }, expression{ expression } {}

		NODE_TYPE(UNARY_OPERATION)
	public:
		Token operator_token;
		ptr<ExpressionNode> expression;
	};

	class BinaryOperationNode : public ExpressionNode {
	public:
		BinaryOperationNode(Token operator_token, ptr<ExpressionNode> left_expression, ptr<ExpressionNode> right_expression)
			: operator_token{ operator_token }, left_expression{ left_expression }, right_expression{ right_expression } {}

		NODE_TYPE(BINARY_OPERATION)
	public:
		Token operator_token;
		ptr<ExpressionNode> left_expression;
		ptr<ExpressionNode> right_expression;
	};

	class IntegerLiteralNode : public ExpressionNode {
	public:
		IntegerLiteralNode(int number) : integer{number} {}

		NODE_TYPE(INT_LITERAL)
	public:
		int integer;
	};

	// Statement Nodes

	using GroupStatement = std::vector<ptr<StatementNode>>;

	class GroupStatementNode : public StatementNode {
	public:
		GroupStatementNode(const GroupStatement& group) : statements{ group } {}

		NODE_TYPE(GROUP_STATEMENT)
	public:
		GroupStatement statements;
	};

	class ReturnStatementNode : public StatementNode {
	public:
		ReturnStatementNode(ptr<ExpressionNode> expr) : expression{ expr } {}

		NODE_TYPE(RETURN_STATEMENT)
	public:
		ptr<ExpressionNode> expression;
	};
}