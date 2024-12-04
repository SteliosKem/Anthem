#include "Parser.h"
#include <iostream>

namespace Anthem {
	// Small Utility
	bool is_binary_operator(Token tok) {
		switch (tok.type)
		{
		case PLUS:
		case MINUS:
		case STAR:
		case SLASH:
		case PERCENT:
			return true;
		default:
			return false;
		}
	}

	uint8_t get_precedence(TokenType token_type) {
		switch (token_type)
		{
		case PLUS:
		case MINUS:
			return 0;
		case STAR:
		case SLASH:
		case PERCENT:
			return 1;
		default:
			break;
		}
	}

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
				std::cout << padding << "Function " << function_node->name << " (\n";
				pretty_print(function_node->body, padding + "\t");
				std::cout << padding << ")\n";
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
		report_error(error_message + ", Got " + current_token().value + " Token");
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
	}

	void Parser::stabilize() {
		// TO-DO
	}

	ptr<ProgramNode> Parser::parse_program() {
		ptr<ProgramNode> program_node = std::make_shared<ProgramNode>();

		while (current_token().type != SPECIAL_EOF && !m_error_occured) {
			program_node->declarations.push_back(parse_declaration());
		}

		return program_node;
	}

	ptr<DeclarationNode> Parser::parse_declaration() {
		//if (match(FUNCTION)) <-- Will be used when global variable declarations are added
		if(consume(FUNCTION, "Expected Function Declaration"))
			return parse_function_declaration();
		return nullptr;
	}

	ptr<DeclarationNode> Parser::parse_function_declaration() {
		// Save identifier
		Token identifier = current_token();
		if (!consume(IDENTIFIER, "Expected Function Identifier")) return nullptr;

		// Handle Arguments
		if (!consume(LEFT_PARENTHESIS, "Expected '('")) return nullptr;
		if (!consume(RIGHT_PARENTHESIS, "Expected ')'")) return nullptr;

		// Parse Function Body - Can be a single statement
		ptr<StatementNode> body = parse_statement();

		return std::make_shared<FunctionDeclarationNode>(identifier.value, body);
	}

// Boilerplate Consume Semicolon on every Statement
#define CONSUME_SEMICOLON() if(!consume(SEMICOLON, "Expected ';'")) return nullptr;
	
	ptr<StatementNode> Parser::parse_statement() {
		switch (current_token().type)
		{
		case RETURN:
			return parse_return_statement();
		// Handle only return statements for now
		default:
			return nullptr;
		}
	}

	ptr<ReturnStatementNode> Parser::parse_return_statement() {
		advance();
		// Parse the expression inside the return statement
		ptr<ExpressionNode> expression = parse_expression();
		
		CONSUME_SEMICOLON()

		return std::make_shared<ReturnStatementNode>(expression);
	}

	ptr<ExpressionNode> Parser::parse_expression(uint8_t mininum_precedence) {
		// Parse expression as left side of a binary operation, even if it won't be one
		ptr<ExpressionNode> left_expression = parse_factor();
		Token operator_token;
		while (is_binary_operator(current_token()) && get_precedence(current_token().type) >= mininum_precedence) {
			operator_token = current_token();
			advance();
			ptr<ExpressionNode> right_expression = parse_expression(get_precedence(operator_token.type) + 1);
			left_expression = std::make_shared<BinaryOperationNode>(operator_token, left_expression, right_expression);
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
			advance();
			return std::make_shared<UnaryOperationNode>(token, parse_factor());
		case LEFT_PARENTHESIS: {
			advance();
			ptr<ExpressionNode> expression = parse_expression();
			consume(RIGHT_PARENTHESIS, "Expected ')'");
			return expression;
		}
		default:
			report_error("Expected Expression");
			return nullptr;
		}
		
	}
}