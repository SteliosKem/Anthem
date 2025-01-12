#include "TypeChecker.h"

namespace Anthem {
	TypeChecker::TypeChecker(ErrorHandler* error_handler) : m_error_handler{ error_handler } {}

	void TypeChecker::check(ptr<ProgramNode> program) {
		for (auto& declaration : program->declarations)
			track_function(declaration);
		for (auto& declaration : program->declarations)
			check_declaration(declaration);
	}

	void TypeChecker::track_function(ptr<DeclarationNode> declaration) {
		if(declaration->get_type() == NodeType::FUNCTION_DECLARATION) {
			auto function = std::static_pointer_cast<FunctionDeclarationNode>(declaration);
			FunctionType func_type;
			func_type.return_type = VarType::I32;
			for (auto& parameter : function->parameters)
				func_type.parameters.push_back(VarType::I32);

			m_symbol_table[function->name] = func_type;
		}
		else if (declaration->get_type() == NodeType::EXTERNAL_DECLARATION) {
			auto function = std::static_pointer_cast<ExternalNode>(declaration);
			FunctionType func_type;
			func_type.is_external = true;
			func_type.return_type = VarType::I32;
			for (auto& parameter : function->parameters)
				func_type.parameters.push_back(VarType::I32);

			m_symbol_table[function->name] = func_type;
		}
	}

	void TypeChecker::check_declaration(ptr<DeclarationNode> declaration) {
		switch (declaration->get_type())
		{
		case NodeType::VARIABLE: {
			auto variable = std::static_pointer_cast<VariableNode>(declaration);
			m_symbol_table[variable->variable_token.value] = VarType::I32;
			if (variable->expression)
				check_expression(variable->expression);
			break;
		}
		case NodeType::FUNCTION_DECLARATION: {
			auto function = std::static_pointer_cast<FunctionDeclarationNode>(declaration);
			check_statement(function->body);
			break; 
		}
		default:
			break;
		}
	}

	void TypeChecker::check_statement(ptr<StatementNode> statement) {
		switch (statement->get_type())
		{
		case NodeType::BLOCK_STATEMENT: {
			ptr<BlockStatementNode> block = std::static_pointer_cast<BlockStatementNode>(statement);
			for (auto& item : block->items) {
				if (std::holds_alternative<ptr<StatementNode>>(item))
					check_statement(std::get<ptr<StatementNode>>(item));
				else
					check_declaration(std::get<ptr<DeclarationNode>>(item));
			}
			break;
		}
		case NodeType::EXPR_STATEMENT: {
			ptr<ExprStatementNode> expr_statement = std::static_pointer_cast<ExprStatementNode>(statement);
			check_expression(expr_statement->expression);
			break;
		}
		case NodeType::FOR_STATEMENT: {
			ptr<ForStatementNode> for_statement = std::static_pointer_cast<ForStatementNode>(statement);
			check_expression(for_statement->condition);
			check_expression(for_statement->init);
			check_expression(for_statement->post_loop);
			check_statement(for_statement->body);
			break;
		}
		case NodeType::WHILE_STATEMENT: {
			ptr<WhileStatementNode> while_statement = std::static_pointer_cast<WhileStatementNode>(statement);
			check_expression(while_statement->condition);
			check_statement(while_statement->body);
			break;
		}
		case NodeType::LOOP_STATEMENT: {
			ptr<LoopStatementNode> loop_statement = std::static_pointer_cast<LoopStatementNode>(statement);
			check_statement(loop_statement->body);
			break;
		}
		case NodeType::IF_STATEMENT: {
			ptr<IfStatementNode> if_statement = std::static_pointer_cast<IfStatementNode>(statement);
			check_expression(if_statement->condition);
			check_statement(if_statement->body);
			if (if_statement->else_body)
				check_statement(if_statement->else_body);
			break;
		}
		case NodeType::BREAK_STATEMENT:
		case NodeType::CONTINUE_STATEMENT:
		case NodeType::VOID_STATEMENT:
		default:
			break;
		}
	}

	void TypeChecker::check_expression(ptr<ExpressionNode> expression) {
		switch (expression->get_type())
		{
		case NodeType::UNARY_OPERATION: {
			ptr<UnaryOperationNode> unary = std::static_pointer_cast<UnaryOperationNode>(expression);
			check_expression(unary->expression);
			break;
		}
		case NodeType::BINARY_OPERATION: {
			ptr<BinaryOperationNode> binary = std::static_pointer_cast<BinaryOperationNode>(expression);
			check_expression(binary->left_expression);
			check_expression(binary->right_expression);
			break;
		}
		case NodeType::ASSIGNMENT: {
			ptr<AssignmentNode> assignment = std::static_pointer_cast<AssignmentNode>(expression);
			check_expression(assignment->lvalue);
			check_expression(assignment->expression);
			break;
		}
		case NodeType::NAME_ACCESS:
			break;
		case NodeType::FUNCTION_CALL: {
			ptr<FunctionCallNode> call = std::static_pointer_cast<FunctionCallNode>(expression);
			auto& function_type = std::get<FunctionType>(m_symbol_table[call->variable_token.value]);
			if (call->argument_list.size() != function_type.parameters.size())
				m_error_handler->report_error(Error{ std::format("Function call '{0}' expected {1} arguments but got {2}"
					, call->variable_token.value, function_type.parameters.size(), call->argument_list.size())});
			for (auto& arg : call->argument_list) {
				check_expression(arg);
			}
			if (function_type.is_external)
				call->is_external = true;
			break;
		}
		default:
			break;
		}
	}
}