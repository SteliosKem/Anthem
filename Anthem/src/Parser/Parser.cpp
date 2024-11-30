#include "Parser.h"

namespace Anthem {
	Parser::Parser(ErrorHandler* error_handler) : m_error_handler{ error_handler } {}

	ptr<ProgramNode> Parser::parse(const TokenList& token_list) {
		m_current_token_index = 0;
	}

	void Parser::advance() {
		if (m_current_token_index + 1 < m_tokens.size())
			m_current_token_index++;
	}

	const Token& Parser::current_token() const {
		return m_tokens[m_current_token_index];
	}

	const Token& Parser::peek(uint32_t depth) const {
		int index = m_current_token_index + depth;
		if (index < m_tokens.size())
			return m_tokens[index];
	}

	bool Parser::consume(TokenType token_type, const std::string& error_message) {
		if (current_token().type == token_type) {
			advance();
			return true;
		}
		m_error_handler->report_error(Error{error_message + ", Got " + current_token().value + " Token", current_token().position});
	}

	void Parser::stabilize() {
		// TO-DO
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