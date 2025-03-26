// SemanticAnalyzer.cpp
// Contains the SemanticAnalyzer Class method implementations
// Copyright (c) 2024-present, Stylianos Kementzetzidis

#include "SemanticAnalyzer.h"

namespace Anthem {
	SemanticAnalyzer::SemanticAnalyzer(ErrorHandler* error_handler) : m_error_handler{ error_handler } {}

	void SemanticAnalyzer::analyze_resolve(ptr<ProgramNode> program_node) {
		m_local_map_stack.push_back({});
		m_global_map.clear();

		// First pass to find all global declarations and handle duplicates
		for (auto& decl : program_node->declarations) {
			save_declaration(decl);
		}
		if (m_error_handler->has_errors())
			return;

		for (auto& decl : program_node->declarations) {
			analyze_declaration(decl);
		}
	}

	SemanticAnalyzer::VarMap& SemanticAnalyzer::current_map() {
		return m_local_map_stack[m_local_map_stack.size() - 1];
	}

	uint64_t SemanticAnalyzer::new_loop() {
		m_loop_stack.push_back(m_loop_counter);
		return m_loop_counter++;
	}

	uint64_t SemanticAnalyzer::current_loop() {
		return m_loop_stack[m_loop_stack.size() - 1];
	}

	void SemanticAnalyzer::save_declaration(ptr<DeclarationNode> declaration_node) {
		switch (declaration_node->get_type())
		{
		case NodeType::VARIABLE: {
			break;
		}
		case NodeType::FUNCTION_DECLARATION: {
			ptr<FunctionDeclarationNode> function = std::static_pointer_cast<FunctionDeclarationNode>(declaration_node);

			// Check if function is already defined
			if (m_global_map.find(function->name) != m_global_map.end())
				report_error("Function '" + function->name + "' is already defined", {});

			m_global_map[function->name] = function->name;
			break;
		}
		default:
			break;
		}
	}

	void SemanticAnalyzer::analyze_declaration(ptr<DeclarationNode> declaration_node) {
		switch (declaration_node->get_type())
		{
		case NodeType::VARIABLE: {
			ptr<VariableNode> variable = std::static_pointer_cast<VariableNode>(declaration_node);
			Name variable_name = variable->variable_token.value;

			// Check if variable already exists in locally or globally
			if (current_map().find(variable_name) != current_map().end()
				|| m_global_map.find(variable_name) != m_global_map.end())
				report_error("Variable '" + variable_name + "' is already defined", variable->variable_token);

			if (variable->expression)
				analyze_expression(variable->expression);

			if (variable->flag == VarFlag::Local) {
				// Add variable to the current local variable map
				Name new_var_name = make_unique(variable_name);
				current_map()[variable_name] = new_var_name;
				variable->variable_token.value = new_var_name;
			}
			// All other VarFlags are handled the same, as globals, except for internal which allows renaming
			else {
				// Add variable to the global variable map
				Name new_var_name = variable->variable_token.value;
				if(variable->flag == VarFlag::Internal)
					new_var_name = make_unique(variable_name);
				m_global_map[variable_name] = new_var_name;
				variable->variable_token.value = new_var_name;
			}
			break;
		}
							   
		case NodeType::FUNCTION_DECLARATION: {
			ptr<FunctionDeclarationNode> function = std::static_pointer_cast<FunctionDeclarationNode>(declaration_node);

			m_global_map[function->name] = function->name;

			if(function->parameters.empty())
				analyze_statement(function->body);
			else {
				// If there are parameters push new scope
				m_local_map_stack.push_back(current_map());
				for (auto& param : function->parameters) {
					// Check if parameter name already exists
					if (current_map().find(param.name) != current_map().end())
						report_error("Variable '" + param.name + "' is already defined", {});

					// Add parameter to the local variable map
					Name new_param_name = make_unique(param.name);
					current_map()[param.name] = new_param_name;
					param.name = new_param_name;
				}
				analyze_statement(function->body);
				m_local_map_stack.pop_back();
			}
			break;
		}
		case NodeType::EXTERNAL_DECLARATION: {
			ptr<ExternalFunctionNode> external = std::static_pointer_cast<ExternalFunctionNode>(declaration_node);

			m_global_map[external->name] = external->name;
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
			m_local_map_stack.push_back(current_map());
			for (auto& block : block_statement->items) {
				if (std::holds_alternative<ptr<StatementNode>>(block))
					analyze_statement(std::get<ptr<StatementNode>>(block));
				else
					analyze_declaration(std::get<ptr<DeclarationNode>>(block));
			}
			m_local_map_stack.pop_back();
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

			// Resolve subexpressions and statements
			analyze_expression(if_statement->condition);
			analyze_statement(if_statement->body);
			if (if_statement->else_body)
				analyze_statement(if_statement->else_body);
			break;
		}
		case NodeType::WHILE_STATEMENT: {
			// Each loop gets a unique id to distinguish break and continue statements
			uint64_t loop_id = new_loop();
			ptr<WhileStatementNode> while_statement = std::static_pointer_cast<WhileStatementNode>(statement);
			while_statement->id = loop_id;

			// Resolve subexpressions and statements
			analyze_expression(while_statement->condition);
			analyze_statement(while_statement->body);

			// Set current loop to the outer loop (if there is one)
			m_loop_stack.pop_back();
			break;
		}
		case NodeType::LOOP_STATEMENT: {
			// Each loop gets a unique id to distinguish break and continue statements
			uint64_t loop_id = new_loop();
			ptr<LoopStatementNode> loop_statement = std::static_pointer_cast<LoopStatementNode>(statement);
			loop_statement->id = loop_id;

			analyze_statement(loop_statement->body);

			// Set current loop to the outer loop (if there is one)
			m_loop_stack.pop_back();
			break;
		}
		case NodeType::FOR_STATEMENT: {
			// Each loop gets a unique id to distinguish break and continue statements
			uint64_t loop_id = new_loop();
			ptr<ForStatementNode> for_statement = std::static_pointer_cast<ForStatementNode>(statement);
			for_statement->id = loop_id;

			// Resolve subexpressions and statements
			analyze_expression(for_statement->init);
			analyze_expression(for_statement->condition);
			analyze_expression(for_statement->post_loop);
			analyze_statement(for_statement->body);

			// Set current loop to the outer loop (if there is one)
			m_loop_stack.pop_back();
			break;
		}
		case NodeType::BREAK_STATEMENT: {
			if (m_loop_stack.empty())
				m_error_handler->report_error(Error{ "Cannot use break outside of a loop" });
			else {
				ptr<BreakStatementNode> break_statement = std::static_pointer_cast<BreakStatementNode>(statement);
				// Attach the id corresponding to the current loop we are in
				break_statement->id = current_loop();
			}
			break;
		}
		case NodeType::CONTINUE_STATEMENT: {
			if (m_loop_stack.empty())
				m_error_handler->report_error(Error{ "Cannot use continue outside of a loop" });
			else {
				ptr<ContinueStatementNode> continue_statement = std::static_pointer_cast<ContinueStatementNode>(statement);
				// Attach the id corresponding to the current loop we are in
				continue_statement->id = current_loop();
			}
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

			// Check if assignment target is an LValue
			if (assignment->lvalue->get_type() != NodeType::NAME_ACCESS)
				report_error("Invalid assignment target", assignment->token);
			analyze_expression(assignment->lvalue);
			analyze_expression(assignment->expression);
			break;
		}
		case NodeType::NAME_ACCESS: {
			ptr<AccessNode> name_access = std::static_pointer_cast<AccessNode>(expression);
			Name name = name_access->variable_token.value;

			// Check if the variable exists
			if (current_map().find(name) != current_map().end())
				name_access->variable_token.value = current_map()[name];
			else if (m_global_map.find(name) != m_global_map.end())
				name_access->variable_token.value = m_global_map[name];
			else
				report_error("Variable '" + name + "' is not defined in this scope", name_access->variable_token);
			
			break;
		}
		case NodeType::FUNCTION_CALL: {
			ptr<FunctionCallNode> function_call = std::static_pointer_cast<FunctionCallNode>(expression);
			Name name = function_call->variable_token.value;

			if (m_global_map.find(name) == m_global_map.end())
				report_error("Function '" + name + "' is not defined", function_call->variable_token);
			else
				name = m_global_map[name];
			
			for (auto& expr : function_call->argument_list) {
				analyze_expression(expr);
			}
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