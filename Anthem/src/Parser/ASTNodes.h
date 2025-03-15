// ASTNodes.h
// Contains Class definitions for the Parser Abstract Syntax Tree Nodes
// Copyright (c) 2024-present, Stylianos Kementzetzidis

#pragma once
#include "Utilities/Utilities.h"
#include "Lexer/Token.h"
#include <variant>

namespace Anthem {
// Macro to simplify basic setup for all node classes
#define NODE_TYPE(x) NodeType get_type() const override { return NodeType::x; }

	enum class NodeType {
		// Groups
			DECLARATION,
			STATEMENT,
			EXPRESSION,

		PROGRAM,
		FUNCTION_DECLARATION,
		EXTERNAL_DECLARATION,
		
		// Expressions
			UNARY_OPERATION,
			BINARY_OPERATION,
			INT_LITERAL,
			ASSIGNMENT,
			VARIABLE,
			NAME_ACCESS,
			FUNCTION_CALL,

		// Statements
			GROUP_STATEMENT,
			RETURN_STATEMENT,
			EXPR_STATEMENT,
			VOID_STATEMENT,
			BLOCK_STATEMENT,
			IF_STATEMENT,
			LOOP_STATEMENT,
			WHILE_STATEMENT,
			FOR_STATEMENT,
			BREAK_STATEMENT,
			CONTINUE_STATEMENT,
			
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

	struct Parameter {
		Name name{ "" };
		ptr<ExpressionNode> default_value{ nullptr };
	};

	// Declaration Nodes

	class FunctionDeclarationNode : public DeclarationNode {
	public:
		FunctionDeclarationNode(const Name& name, ptr<StatementNode> body, const std::vector<Parameter>& parameters, ReturnType type = ReturnType::I32)
			: name{ name }, body{ body }, parameters{ parameters }, return_type{ type } {}

		NODE_TYPE(FUNCTION_DECLARATION)
	public:
		Name name;
		std::vector<Parameter> parameters;
		ptr<StatementNode> body;
		ReturnType return_type;
		VarFlag flag;
	};

	class ExternalFunctionNode : public DeclarationNode {
	public:
		ExternalFunctionNode(const Name& name, const std::vector<Parameter>& parameters, ReturnType type = ReturnType::I32)
			: name{ name }, parameters{ parameters }, return_type{ type } {}

		NODE_TYPE(EXTERNAL_DECLARATION)
	public:
		Name name;
		std::vector<Parameter> parameters;
		ReturnType return_type;
	};

	class VariableNode : public DeclarationNode {
	public:
		VariableNode(Token variable_token, ReturnType type = ReturnType::I32, ptr<ExpressionNode> expression = nullptr, VarFlag flag = VarFlag::Local)
			: variable_token{ variable_token }, expression{ expression }, type{ type }, flag{ flag } {}

		NODE_TYPE(VARIABLE)
	public:
		Token variable_token;
		ptr<ExpressionNode> expression;
		ReturnType type;
		VarFlag flag;
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
		BinaryOperationNode(const Token& operator_token, ptr<ExpressionNode> left_expression, ptr<ExpressionNode> right_expression)
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

	class AssignmentNode : public ExpressionNode {
	public:
		AssignmentNode(ptr<ExpressionNode> lvalue, ptr<ExpressionNode> expression, const Token& equals_token) 
			: lvalue{ lvalue }, expression{ expression }, token{ token } {}

		NODE_TYPE(ASSIGNMENT)
	public:
		ptr<ExpressionNode> lvalue;
		ptr<ExpressionNode> expression;
		Token token;
	};

	class AccessNode : public ExpressionNode {
	public:
		AccessNode(Token variable_token) : variable_token{ variable_token } {}

		NODE_TYPE(NAME_ACCESS)
	public:
		Token variable_token;
	};

	using ArgList = std::vector<ptr<ExpressionNode>>;

	class FunctionCallNode : public ExpressionNode {
	public:
		FunctionCallNode(Token variable_token, const ArgList& argument_list) 
			: variable_token{ variable_token }, argument_list{ argument_list } {}

		NODE_TYPE(FUNCTION_CALL)
	public:
		Token variable_token;
		ArgList argument_list;
		bool is_external = false;
	};

	// Statement Nodes

	class ReturnStatementNode : public StatementNode {
	public:
		ReturnStatementNode(ptr<ExpressionNode> expr) : expression{ expr } {}

		NODE_TYPE(RETURN_STATEMENT)
	public:
		ptr<ExpressionNode> expression;
	};

	class ExprStatementNode : public StatementNode {
	public:
		ExprStatementNode(ptr<ExpressionNode> expr) : expression{ expr } {}

		NODE_TYPE(EXPR_STATEMENT)
	public:
		ptr<ExpressionNode> expression;
	};

	class VoidStatementNode : public StatementNode {
	public:
		NODE_TYPE(VOID_STATEMENT)
	};

	using BlockItem = std::variant<ptr<DeclarationNode>, ptr<StatementNode>>;
	using BlockItems = std::vector<BlockItem>;

	class BlockStatementNode : public StatementNode {
	public:
		BlockStatementNode() = default;
		BlockStatementNode(const BlockItems& items) : items{ items } {}

		NODE_TYPE(BLOCK_STATEMENT)
	public:
		BlockItems items;
	};

	class IfStatementNode : public StatementNode {
	public:
		IfStatementNode() = default;
		IfStatementNode(ptr<ExpressionNode> condition, ptr<StatementNode> body, ptr<StatementNode> else_body)
			: condition{ condition }, body{ body }, else_body{ else_body } {}

		NODE_TYPE(IF_STATEMENT)
	public:
		ptr<ExpressionNode> condition;
		ptr<StatementNode> body;

		// If there is no else statement, set to nullptr
		ptr<StatementNode> else_body = nullptr;
	};

	class LoopStatementNode : public StatementNode {
	public:
		LoopStatementNode() = default;
		LoopStatementNode(ptr<StatementNode> body) : body{ body } {}

		NODE_TYPE(LOOP_STATEMENT)
	public:
		ptr<StatementNode> body;

		// ID will be provided in the semantic analysis pass
		uint64_t id;
	};

	class WhileStatementNode : public StatementNode {
	public:
		WhileStatementNode() = default;
		WhileStatementNode(ptr<ExpressionNode> condition, ptr<StatementNode> body) : condition{ condition }, body { body } {}

		NODE_TYPE(WHILE_STATEMENT)
	public:
		ptr<ExpressionNode> condition;
		ptr<StatementNode> body;

		// ID will be provided in the semantic analysis pass
		uint64_t id;
	};

	class ForStatementNode : public StatementNode {
	public:
		ForStatementNode() = default;
		ForStatementNode(ptr<ExpressionNode> init, ptr<ExpressionNode> condition, ptr<ExpressionNode> post_loop, ptr<StatementNode> body)
			: condition{ condition }, body{ body }, init{ init }, post_loop{ post_loop } {}

		NODE_TYPE(FOR_STATEMENT)
	public:
		ptr<ExpressionNode> init;
		ptr<ExpressionNode> post_loop;
		ptr<ExpressionNode> condition;
		ptr<StatementNode> body;

		// ID will be provided in the semantic analysis pass
		uint64_t id;
	};

	class BreakStatementNode : public StatementNode {
	public:
		NODE_TYPE(BREAK_STATEMENT)
		uint64_t id;
	};

	class ContinueStatementNode : public StatementNode {
	public:
		NODE_TYPE(CONTINUE_STATEMENT)
		uint64_t id;
	};
}