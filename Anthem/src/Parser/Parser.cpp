#include "Parser.h"

namespace Anthem {
	Parser::Parser(ErrorHandler* error_handler) : m_error_handler{ error_handler } {}

	ptr<ProgramNode> Parser::parse(const TokenList& token_list) {

	}

	void Parser::advance() {

	}

	const Token& Parser::current_token() const {

	}

	const Token& Parser::peek(uint32_t depth) const {

	}

	void Parser::consume(TokenType token_type, const std::string& error_message) {

	}

	void Parser::stabilize() {

	}

	ptr<ProgramNode> Parser::parse_program() {

	}

	ptr<DeclarationNode> Parser::parse_declaration() {

	}
	
	ptr<StatementNode> Parser::parse_statement() {

	}

	ptr<ReturnStatementNode> Parser::parse_return_statement() {

	}

	ptr<ExpressionNode> Parser::parse_expression() {

	}

	ptr<ExpressionNode> Parser::parse_factor() {

	}
}