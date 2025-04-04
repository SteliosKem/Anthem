// Parser.cpp
// Contains the Parser Class method implementations
// Copyright (c) 2024-present, Stylianos Kementzetzidis

#include "Parser.h"
#include <iostream>

namespace Anthem {
	// Simple Utility
	bool is_binary_operator(Token tok) {
		switch (tok.type)
		{
		case PLUS:
		case MINUS:
		case STAR:
		case SLASH:
		case PERCENT:
		case GREATER:
		case GREATER_EQUAL:
		case LESS:
		case LESS_EQUAL:
		case EQUAL:
		case EQUAL_EQUAL:
		case BANG_EQUAL:
		case AND:
		case OR:
			return true;
		default:
			return false;
		}
	}

	// Operator precedence, higher precedence = will get resolved first
	uint8_t get_precedence(TokenType token_type) {
		switch (token_type)
		{
		case PLUS:
		case MINUS:
			return 5;

		case STAR:
		case SLASH:
		case PERCENT:
			return 6;

		case LESS:
		case GREATER:
		case LESS_EQUAL:
		case GREATER_EQUAL:
			return 4;

		case EQUAL_EQUAL:
		case BANG_EQUAL:
			return 3;

		case AND:
			return 2;
		case OR:
			return 1;
		case EQUAL:
			return 0;
		default:
			break;
		}
	}

	// Boilerplate Consume Semicolon on every Statement
#define CONSUME_SEMICOLON() if(!consume(SEMICOLON, "Expected ';'")) return nullptr;


	Parser::Parser(ErrorHandler* error_handler) : m_error_handler{ error_handler } {}

	ptr<ProgramNode> Parser::parse(const TokenList& token_list) {
		m_current_token_index = -1;
		m_tokens = token_list;
		advance();
		return parse_program();
	}

	void Parser::pretty_print(ptr<ASTNode> node, const std::string& padding) {
		switch (node->get_type())
		{
			case NodeType::PROGRAM: {
				ptr<ProgramNode> program_node = std::static_pointer_cast<ProgramNode>(node);
				std::cout << "Program (\n";
				for (auto& statement : program_node->declarations) {
					pretty_print(statement, "\t");
				}
				std::cout << ")";
				break;
			}
			case NodeType::FUNCTION_DECLARATION: {
				ptr<FunctionDeclarationNode> function_node = std::static_pointer_cast<FunctionDeclarationNode>(node);
				std::cout << padding << "Function " << int(function_node->return_type) << " " << function_node->name << " (";
				for (auto& i : function_node->parameters) {
					std::cout << i.name << ", " << int(i.type) << " ";
				}
				std::cout << ") (\n";
				pretty_print(function_node->body, padding + "\t");
				std::cout << padding << ")\n";
				break;
			}
			case NodeType::EXTERNAL_DECLARATION: {
				ptr<ExternalFunctionNode> external_node = std::static_pointer_cast<ExternalFunctionNode>(node);
				std::cout << padding << "External Function " << external_node->name << " (";
				for (auto& i : external_node->parameters) {
					std::cout << i.name << ", ";
				}
				std::cout << ")\n";
				break;
			}
			case NodeType::RETURN_STATEMENT: {
				ptr<ReturnStatementNode> return_node = std::static_pointer_cast<ReturnStatementNode>(node);
				std::cout << padding << "Return ";
				pretty_print(return_node->expression);
				std::cout << "\n";
				break;
			}
			case NodeType::INT_LITERAL: {
				std::cout << std::static_pointer_cast<IntegerLiteralNode>(node)->integer;
				break;
			}
			case NodeType::UNARY_OPERATION: {
				ptr<UnaryOperationNode> unary_op = std::static_pointer_cast<UnaryOperationNode>(node);
				std::cout << unary_op->operator_token.value << '(';
				pretty_print(unary_op->expression);
				std::cout << ')';
				break;
			}
			case NodeType::BINARY_OPERATION: {
				ptr<BinaryOperationNode> binary_op = std::static_pointer_cast<BinaryOperationNode>(node);
				std::cout << '(';
				pretty_print(binary_op->left_expression);
				std::cout << ' ';
				std::cout << binary_op->operator_token.value;
				std::cout << ' ';
				pretty_print(binary_op->right_expression);
				std::cout << ')';
				break;
			}
			case NodeType::NAME_ACCESS: {
				ptr<AccessNode> access = std::static_pointer_cast<AccessNode>(node);
				std::cout << "Access(" << access->variable_token.value << ")";
				break;
			}
			case NodeType::EXPR_STATEMENT: {
				ptr<ExprStatementNode> expression = std::static_pointer_cast<ExprStatementNode>(node);
				std::cout << padding << "Expression ";
				pretty_print(expression->expression);
				std::cout << "\n";
				break;
			}
			case NodeType::VOID_STATEMENT: {
				std::cout << padding << "Void\n";
				break;
			}
			case NodeType::VARIABLE: {
				ptr<VariableNode> variable = std::static_pointer_cast<VariableNode>(node);
				std::cout << padding << "Variable Declaration " << variable->variable_token.value << " " << int(variable->type);
				if (variable->expression) {
					pretty_print(variable->expression);
					std::cout << "\n";
				}
				break;
			}
			case NodeType::ASSIGNMENT: {
				ptr<AssignmentNode> assignment = std::static_pointer_cast<AssignmentNode>(node);
				std::cout << "Assign ";
				pretty_print(assignment->expression);
				std::cout << " to ";
				pretty_print(assignment->lvalue);
				break;
			}
			case NodeType::BLOCK_STATEMENT: {
				ptr<BlockStatementNode> block = std::static_pointer_cast<BlockStatementNode>(node);
				for(auto& item : block->items) {
					if (std::holds_alternative<ptr<StatementNode>>(item))
						pretty_print(std::get<ptr<StatementNode>>(item), padding);
					else 
						pretty_print(std::get<ptr<DeclarationNode>>(item), padding);
				}
				break;
			}
			case NodeType::IF_STATEMENT: {
				ptr<IfStatementNode> if_statement = std::static_pointer_cast<IfStatementNode>(node);
				std::cout << padding << "If: ";
				pretty_print(if_statement->condition);
				std::cout << " then:\n";
				pretty_print(if_statement->body, padding + "\t");
				std::cout << "\n";
				if (if_statement->else_body) {
					std::cout << padding << "Else:\n";
					pretty_print(if_statement->else_body, padding + "\t");
					std::cout << "\n";
				}
				break;
			}
			case NodeType::LOOP_STATEMENT: {
				ptr<LoopStatementNode> loop_statement = std::static_pointer_cast<LoopStatementNode>(node);
				std::cout << padding << "Loop:\n";
				pretty_print(loop_statement->body, padding + "\t");
				break;
			}
			case NodeType::WHILE_STATEMENT: {
				ptr<WhileStatementNode> while_statement = std::static_pointer_cast<WhileStatementNode>(node);
				std::cout << padding << "While: ";
				pretty_print(while_statement->condition);
				std::cout << " do:\n";
				pretty_print(while_statement->body, padding + "\t");
				break;
			}
			case NodeType::FOR_STATEMENT: {
				ptr<ForStatementNode> for_statement = std::static_pointer_cast<ForStatementNode>(node);
				std::cout << padding << "For: Init: ";
				pretty_print(for_statement->init);
				std::cout << " While ";
				pretty_print(for_statement->condition);
				std::cout << " Post ";
				pretty_print(for_statement->post_loop);
				std::cout << " do:\n";
				pretty_print(for_statement->body, padding + "\t");
				break;
			}
			case NodeType::FUNCTION_CALL: {
				ptr<FunctionCallNode> call = std::static_pointer_cast<FunctionCallNode>(node);
				std::cout << padding << "Call: " << call->variable_token.value << " (";
				for (auto& expr : call->argument_list) {
					pretty_print(expr);
					std::cout << ", ";
				}
				std::cout << ")";
				break;
			}
		default:
			break;
		}
	}

	void Parser::advance() {
		m_current_token_index++;
		if (m_current_token_index < m_tokens.size())
			m_current_token = &m_tokens[m_current_token_index];
		else
			m_current_token = &m_tokens[m_tokens.size() - 1];
			
	}

	const Token& Parser::current_token() const {
		return *m_current_token;
	}

	const Token& Parser::peek(uint32_t depth) const {
		int index = m_current_token_index + depth;
		if (index < m_tokens.size())
			return m_tokens[index];
	}

	bool Parser::consume(TokenType token_type, const std::string& error_message) {
		if (match(token_type)) return true;
		report_error(error_message + ", got " + current_token().value + " Token");
		return false;
	}

	bool Parser::match(TokenType token_type) {
		if (is_current(token_type)) {
			advance();
			return true;
		}
		return false;
	}

	bool Parser::is_current(TokenType token_type) const {
		return current_token().type == token_type;
	}

	void Parser::report_error(const std::string& error_message) {
		m_error_handler->report_error(Error{ error_message, current_token().position });
		m_error_occured = true;
		stabilize();
	}

	void Parser::stabilize() {
		while (current_token().type != TokenType::SPECIAL_EOF) {
			const Token& tok = current_token();

			// Stop skipping tokens if a semicolon or certain keyword is found
			if (tok.type == TokenType::SEMICOLON) {
				advance();
				return;
			}
			if (tok.value == "if" || tok.value == "fn" || tok.value == "let"
				|| tok.value == "while" || tok.value == "for" || tok.value == "loop" || tok.type == TokenType::LEFT_BRACE)
				return;
			advance();
		}
	}

	ptr<ProgramNode> Parser::parse_program() {
		ptr<ProgramNode> program_node = std::make_shared<ProgramNode>();

		while (current_token().type != SPECIAL_EOF && !m_error_occured) {
			program_node->declarations.push_back(parse_declaration());
		}

		return program_node;
	}

	BlockItem Parser::parse_block_item() {
		if (is_current(LET))
			return parse_variable_declaration();
		return parse_statement();
	}

	ptr<DeclarationNode> Parser::parse_declaration() {
		switch (current_token().type) {
		case FUNCTION:
			advance();
			return parse_function_declaration();
		case EXTERNAL:
			if (peek().type == FUNCTION) {
				advance();
				advance();
				return parse_external();
			}
			return parse_variable_declaration(VarFlag::External);
		case INTERNAL:
			if (peek().type == FUNCTION) {
				advance();
				advance();
				return parse_function_declaration(VarFlag::Internal);
			}
			return parse_variable_declaration(VarFlag::Internal);
		case GLOBAL:
			return parse_variable_declaration(VarFlag::Global);
		default:
			report_error("Expected a declaration");
		}
		return nullptr;
	}

	ptr<DeclarationNode> Parser::parse_function_declaration(VarFlag flag) {
		// Save identifier
		Token identifier = current_token();
		if (!consume(IDENTIFIER, "Expected Function Identifier")) return nullptr;

		// Handle Arguments
		if (!consume(LEFT_PARENTHESIS, "Expected '('")) return nullptr;
		std::vector<Parameter> parameter_list;

		while (!is_current(RIGHT_PARENTHESIS) && !is_current(SPECIAL_EOF)) {
			Token current_tok = current_token();
			consume(IDENTIFIER, "Expected Parameter Name");

			ReturnType type;

			if (!match(COLON))
				report_error("Expected ':'");
			if (is_type_token(current_token()))
				type = get_type(current_token());
			else
				report_error("Expected Type after ':'");
			advance();

			parameter_list.push_back({ current_tok.value, type });

			if (is_current(COMMA))
				advance();
			else
				break;
		}

		if (!consume(RIGHT_PARENTHESIS, "Expected ')'")) return nullptr;

		ReturnType type{};

		if (!match(COLON))
			report_error("Expected ':'");
		if (is_type_token(current_token()))
			type = get_type(current_token());
		else
			report_error("Expected Type after ':'");
		advance();

		// Parse Function Body - Can be a single statement
		ptr<StatementNode> body = parse_statement();

		ptr<FunctionDeclarationNode> func = std::make_shared<FunctionDeclarationNode>(identifier.value, body, parameter_list);
		func->return_type = type;
		func->flag = flag;

		return func;
	}

	ptr<DeclarationNode> Parser::parse_external() {
		// Save identifier
		Token identifier = current_token();
		if (!consume(IDENTIFIER, "Expected Function Identifier")) return nullptr;

		// Handle Arguments
		if (!consume(LEFT_PARENTHESIS, "Expected '('")) return nullptr;
		std::vector<Parameter> parameter_list;

		while (!is_current(RIGHT_PARENTHESIS) && !is_current(SPECIAL_EOF)) {
			Token current_tok = current_token();
			consume(IDENTIFIER, "Expected Parameter Name");

			ReturnType type;

			if (!match(COLON))
				report_error("Expected ':'");
			if (is_type_token(current_token()))
				type = get_type(current_token());
			else
				report_error("Expected Type after ':'");
			advance();

			parameter_list.push_back({ current_tok.value, type });

			if (is_current(COMMA))
				advance();
			else
				break;
		}

		if (!consume(RIGHT_PARENTHESIS, "Expected ')'")) return nullptr;

		ReturnType type{};

		if (!match(COLON))
			report_error("Expected ':'");
		if (is_type_token(current_token()))
			type = get_type(current_token());
		else
			report_error("Expected Type after ':'");
		advance();

		CONSUME_SEMICOLON();

		return std::make_shared<ExternalFunctionNode>(identifier.value, parameter_list, type);
	}

	ptr<DeclarationNode> Parser::parse_variable_declaration(VarFlag flag) {
		advance();
		Token identifier_token = current_token();
		consume(IDENTIFIER, "Expected identifier");
		ptr<VariableNode> variable = nullptr;
		ReturnType type;
		if (!match(COLON))
			report_error("Expected ':'");
		if (is_type_token(current_token()))
			type = get_type(current_token());
		else
			report_error("Expected Type after ':'");

		advance();

		// If there is assignment parse the expression given
		if (match(EQUAL))
			variable = std::make_shared<VariableNode>(identifier_token, type, parse_expression());
		else
			variable = std::make_shared<VariableNode>(identifier_token, type);

		variable->flag = flag;

		CONSUME_SEMICOLON();

		return variable;
	}

	ptr<StatementNode> Parser::parse_statement() {
		switch (current_token().type)
		{
		case RETURN:
			return parse_return_statement();
		case IF:
			return parse_if_statement();
		case WHILE:
			return parse_while_statement();
		case FOR:
			return parse_for_statement();
		case LOOP:
			return parse_loop_statement();
		case BREAK:
			advance();
			CONSUME_SEMICOLON();
			return std::make_shared<BreakStatementNode>();
		case CONTINUE:
			advance();
			CONSUME_SEMICOLON();
			return std::make_shared<ContinueStatementNode>();
		case LEFT_BRACE:
			return parse_block_statement();
		case SEMICOLON:
			advance();
			return std::make_shared<VoidStatementNode>();
		// If nothing matches, then it probably will be an expression statement
		default:
			return parse_expr_statement();
		}
	}

	ptr<ReturnStatementNode> Parser::parse_return_statement() {
		advance();
		// Parse the expression inside the return statement
		ptr<ExpressionNode> expression = parse_expression();
		
		CONSUME_SEMICOLON()

		return std::make_shared<ReturnStatementNode>(expression);
	}

	ptr<BlockStatementNode> Parser::parse_block_statement() {
		advance();
		auto block_statement = std::make_shared<BlockStatementNode>();
		// Parse the expression inside the return statement
		while(!is_current(RIGHT_BRACE) && !is_current(SPECIAL_EOF))
			block_statement->items.push_back(parse_block_item());

		consume(RIGHT_BRACE, "Expected '}'");

		return block_statement;
	}

	ptr<ExprStatementNode> Parser::parse_expr_statement() {
		ptr<ExpressionNode> expression = parse_expression();

		CONSUME_SEMICOLON();

		return std::make_shared<ExprStatementNode>(expression);
	}

	ptr<IfStatementNode> Parser::parse_if_statement() {
		advance();
		ptr<ExpressionNode> condition = parse_expression();

		consume(ARROW, "Expected '->' after if condition");

		ptr<StatementNode> if_body = parse_statement();
		ptr<StatementNode> else_body = nullptr;

		if (match(ELSE))
			else_body = parse_statement();

		return std::make_shared<IfStatementNode>(condition, if_body, else_body);
	}

	ptr<LoopStatementNode> Parser::parse_loop_statement() {
		advance();
		return std::make_shared<LoopStatementNode>(parse_statement());
	}

	ptr<WhileStatementNode> Parser::parse_while_statement() {
		advance();
		ptr<ExpressionNode> condition = parse_expression();

		consume(ARROW, "Expected '->' after if condition");

		ptr<StatementNode> while_body = parse_statement();

		return std::make_shared<WhileStatementNode>(condition, while_body);
	}

	ptr<ForStatementNode> Parser::parse_for_statement() {
		advance();
		ptr<ExpressionNode> init = parse_expression();
		consume(SEMICOLON, "Expected ';'");

		ptr<ExpressionNode> condition = parse_expression();
		consume(SEMICOLON, "Expected ';'");

		ptr<ExpressionNode> post_loop = parse_expression();

		consume(ARROW, "Expected '->'");

		ptr<StatementNode> for_body = parse_statement();

		return std::make_shared<ForStatementNode>(init, condition, post_loop, for_body);
	}

	ptr<ExpressionNode> Parser::parse_expression(uint8_t mininum_precedence) {
		// Parse expression as left side of a binary operation, even if it won't be one
		ptr<ExpressionNode> left_expression = parse_factor();
		Token operator_token;
		while (is_binary_operator(current_token()) && get_precedence(current_token().type) >= mininum_precedence) {
			operator_token = current_token();
			advance();
			if (operator_token.type == EQUAL) {
				ptr<ExpressionNode> right_expression = parse_expression(get_precedence(operator_token.type));
				left_expression = std::make_shared<AssignmentNode>(left_expression, right_expression, operator_token);
			}
			else {
				ptr<ExpressionNode> right_expression = parse_expression(get_precedence(operator_token.type) + 1);
				left_expression = std::make_shared<BinaryOperationNode>(operator_token, left_expression, right_expression);
			}
		}
		return left_expression;
	}

	ptr<ExpressionNode> Parser::parse_factor() {
		Token token = current_token();
		switch (token.type) {
		case TYPE_I32:
			// Make Integer Literal from current token value
			advance();
			return std::make_shared<IntegerLiteralNode>(stoi(token.value));

		// All these Tokens when in parse_factor make up unary operations
		case MINUS:
		case PLUS:
		case TILDE:
		case BANG:
			advance();
			return std::make_shared<UnaryOperationNode>(token, parse_factor());
		case LEFT_PARENTHESIS: {
			advance();
			ptr<ExpressionNode> expression = parse_expression();
			consume(RIGHT_PARENTHESIS, "Expected ')'");
			return expression;
		}
		case IDENTIFIER: {
			advance();
			
			if (match(LEFT_PARENTHESIS)) {
				ArgList argument_list;
				while (!is_current(RIGHT_PARENTHESIS) && !is_current(SPECIAL_EOF)) {
					argument_list.push_back(parse_expression());

					if (is_current(COMMA))
						advance();
					else
						break;
				}

				if (!consume(RIGHT_PARENTHESIS, "Expected ')'")) return nullptr;


				return std::make_shared<FunctionCallNode>(token, argument_list);
			}
			return std::make_shared<AccessNode>(token);
		}
		default:
			report_error("Expected Expression");
			return nullptr;
		}
		
	}
}