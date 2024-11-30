#pragma once
#include "Utilities/Error.h"
#include "ASTNodes.h"
#include "Lexer/Token.h"

namespace Anthem {
	class Parser {
	public:
		Parser(ErrorHandler* handler);

		ptr<ProgramNode> parse(const TokenList& tokens);
	private:
		// Utility

		// Advance to next token if it exists
		void advance();

		// Get Token at current Index
		const Token& current_token() const;

		// Get Token at <depth> amount after current Index
		const Token& peek(uint32_t depth = 1) const;

		// Advance if current Token is of desired type, else report an Error for Unexpected Token
		void consume(TokenType token_type, const std::string& error_message);

		// Called if an error occurs, skips Tokens in order to get to an Error-free state
		void stabilize();
	private:
		// Program Parsing
		
		ptr<ProgramNode> parse_program();

		// Declaration Parsing

		ptr<DeclarationNode> parse_declaration();

		// Statement Parsing

		ptr<StatementNode> parse_statement();
		ptr<ReturnStatementNode> parse_return_statement();

		// Expression Parsing

		ptr<ExpressionNode> parse_expression();
		ptr<ExpressionNode> parse_factor();

	private:
		int m_current_token_index;
		ErrorHandler* m_error_handler;

		TokenList m_tokens;
	};
}