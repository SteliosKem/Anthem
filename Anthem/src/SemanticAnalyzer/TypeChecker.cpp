// TypeChecker.cpp
// Contains the TypeChecker Class method implementations
// Copyright (c) 2024-present, Stylianos Kementzetzidis

#include "TypeChecker.h"

namespace Anthem {
	TypeChecker::TypeChecker(ErrorHandler* error_handler) : m_error_handler{ error_handler } {}

	void TypeChecker::check(ptr<ProgramNode> program) {
		m_symbol_table.clear();
		for (auto& declaration : program->declarations)
			track_function(declaration);
		for (auto& declaration : program->declarations)
			check_declaration(declaration);
	}

	void TypeChecker::track_function(ptr<DeclarationNode> declaration) {
		if(declaration->get_type() == NodeType::FUNCTION_DECLARATION) {
			auto function = std::static_pointer_cast<FunctionDeclarationNode>(declaration);
			FunctionType func_type;

			// Only 32 bit integer types for now
			func_type.return_type = ReturnType::I32;
			for (auto& parameter : function->parameters)
				func_type.parameters.push_back(ReturnType::I32);

			// Add the function to the symbol table
			m_symbol_table[function->name] = func_type;
		}
		else if (declaration->get_type() == NodeType::EXTERNAL_DECLARATION) {
			auto function = std::static_pointer_cast<ExternalFunctionNode>(declaration);
			FunctionType func_type;
			func_type.is_external = true;
			func_type.return_type = ReturnType::I32;
			for (auto& parameter : function->parameters)
				func_type.parameters.push_back(ReturnType::I32);

			// Add the external function to the symbol table
			m_symbol_table[function->name] = func_type;
		}
	}

	void TypeChecker::check_declaration(ptr<DeclarationNode> declaration) {
		switch (declaration->get_type())
		{
		case NodeType::VARIABLE: {
			auto variable = std::static_pointer_cast<VariableNode>(declaration);

			int initializer = 0;

			if (variable->flag != VarFlag::Local && variable->expression) {
				if(variable->flag == VarFlag::External)
					m_error_handler->report_error(Error{ "External variable declarations cannot have an initializer" });
				else {
					if (variable->expression->get_type() != NodeType::INT_LITERAL)
						m_error_handler->report_error(Error{ "Global and internal variable declarations cannot have a non-constant initializer" });
				}
				initializer = std::static_pointer_cast<IntegerLiteralNode>(variable->expression)->integer;
			}

			// Only 32 bit integer types for now
			m_symbol_table[variable->variable_token.value] = VariableType{ ReturnType::I32, variable->flag, initializer };
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
		// Type check subexpressions
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

			// Get the function type object of the called function from the symbol table
			auto& function_type = std::get<FunctionType>(m_symbol_table[call->variable_token.value]);

			// If the number of arguments don't match the number of parameters throw an error
			if (call->argument_list.size() != function_type.parameters.size())
				m_error_handler->report_error(Error{ std::format("Function call '{0}' expected {1} arguments but got {2}"
					, call->variable_token.value, function_type.parameters.size(), call->argument_list.size())});

			// Type check arguments
			for (auto& arg : call->argument_list) {
				check_expression(arg);
			}

			// Set external status
			if (function_type.is_external)
				call->is_external = true;
			break;
		}
		default:
			break;
		}
	}
}