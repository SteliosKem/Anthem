#include "SemanticAnalyzer.h"

namespace Anthem {
	SemanticAnalyzer::SemanticAnalyzer(ErrorHandler* error_handler) : m_error_handler{ error_handler } {}

	void SemanticAnalyzer::analyze_resolve(ptr<ProgramNode> program_node) {
		for (auto& decl : program_node->declarations) {
			analyze_declaration(decl);
		}
	}

	void SemanticAnalyzer::analyze_declaration(ptr<DeclarationNode> declaration_node) {
		switch (declaration_node->get_type())
		{
		case NodeType::VARIABLE: {
			ptr<VariableNode> variable = std::static_pointer_cast<VariableNode>(declaration_node);
			Name variable_name = variable->variable_token.value;
			if (m_local_map.find(variable_name) != m_local_map.end())
				report_error("Variable '" + variable_name + "' is already defined", variable->variable_token);

			if (variable->expression)
				analyze_expression(variable->expression);

			Name new_var_name = make_unique(variable_name);
			m_local_map[variable_name] = new_var_name;
			variable->variable_token.value = new_var_name;
			break;
		}
							   
		case NodeType::FUNCTION_DECLARATION: {
			ptr<FunctionDeclarationNode> function = std::static_pointer_cast<FunctionDeclarationNode>(declaration_node);
			analyze_statement(function->body);
			break;
		}
		default:
			break;
		}
	}

	void SemanticAnalyzer::analyze_statement(ptr<StatementNode> statement) {
		switch (statement->get_type())
		{
		case NodeType::BLOCK_STATEMENT: {
			ptr<BlockStatementNode> block_statement = std::static_pointer_cast<BlockStatementNode>(statement);
			for (auto& block : block_statement->items) {
				if (std::holds_alternative<ptr<StatementNode>>(block))
					analyze_statement(std::get<ptr<StatementNode>>(block));
				else
					analyze_declaration(std::get<ptr<DeclarationNode>>(block));
			}
			break;
		}
		case NodeType::EXPR_STATEMENT: {
			ptr<ExprStatementNode> expr_statement = std::static_pointer_cast<ExprStatementNode>(statement);
			analyze_expression(expr_statement->expression);
			break;
		}
		case NodeType::RETURN_STATEMENT: {
			ptr<ReturnStatementNode> return_statement = std::static_pointer_cast<ReturnStatementNode>(statement);
			analyze_expression(return_statement->expression);
			break;
		}
		case NodeType::IF_STATEMENT: {
			ptr<IfStatementNode> if_statement = std::static_pointer_cast<IfStatementNode>(statement);
			analyze_expression(if_statement->condition);
			analyze_statement(if_statement->body);
			if (if_statement->else_body)
				analyze_statement(if_statement->else_body);
			break;
		}
		default:
			break;
		}
	}

	void SemanticAnalyzer::analyze_expression(ptr<ExpressionNode> expression) {
		switch (expression->get_type()) {
		case NodeType::UNARY_OPERATION: {
			ptr<UnaryOperationNode> unary_op = std::static_pointer_cast<UnaryOperationNode>(expression);
			analyze_expression(unary_op->expression);
			break;
		}
		case NodeType::BINARY_OPERATION: {
			ptr<BinaryOperationNode> binary_op = std::static_pointer_cast<BinaryOperationNode>(expression);
			analyze_expression(binary_op->left_expression);
			analyze_expression(binary_op->right_expression);
			break;
		}
		case NodeType::ASSIGNMENT: {
			ptr<AssignmentNode> assignment = std::static_pointer_cast<AssignmentNode>(expression);
			if (assignment->lvalue->get_type() != NodeType::NAME_ACCESS)
				report_error("Invalid assignment target", assignment->token);
			analyze_expression(assignment->lvalue);
			analyze_expression(assignment->expression);
			break;
		}
		case NodeType::NAME_ACCESS: {
			ptr<AccessNode> name_access = std::static_pointer_cast<AccessNode>(expression);
			Name name = name_access->variable_token.value;
			if (m_local_map.find(name) == m_local_map.end())
				report_error("Undeclared variable '" + name + "'", name_access->variable_token);
			else
				name_access->variable_token.value = m_local_map[name];
			break;
		}
		}
	}

	void SemanticAnalyzer::report_error(const std::string& error_msg, const Token& token) {
		m_error_handler->report_error(Error{ error_msg, token.position });
	}

	Name SemanticAnalyzer::make_unique(const Name& name) {
		return name + "#" + std::to_string(m_unique_counter++);
	}
}