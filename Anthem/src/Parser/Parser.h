#pragma once
#include "Utilities/Error.h"
#include "ASTNodes.h"
#include "Lexer/Token.h"

namespace Anthem {
	class Parser {
	public:
		Parser(ErrorHandler* handler);

		ptr<ProgramNode> parse(const TokenList& tokens);
		static void pretty_print(ptr<ASTNode> node, const std::string& padding = "");
	private:
		// Utility

		// Advance to next token if it exists
		void advance();

		// Get Token at current Index
		const Token& current_token() const;

		// Get Token at <depth> amount after current Index
		const Token& peek(uint32_t depth = 1) const;

		// Advance if current Token is of desired type, else report an Error for Unexpected Token
		bool consume(TokenType token_type, const std::string& error_message);

		// Return true and advance if current token matches the specified type
		bool match(TokenType token_type);
		bool is_current(TokenType token_type) const;

		// Called if an error occurs, skips Tokens in order to get to an Error-free state
		void stabilize();

		// Error Handling
		void report_error(const std::string& error_message);
	private:
		// Program Parsing
		
		ptr<ProgramNode> parse_program();

		// Miscellaneous

		BlockItem parse_block_item();

		// Declaration Parsing

		ptr<DeclarationNode> parse_declaration();
		ptr<DeclarationNode> parse_function_declaration();
		ptr<DeclarationNode> parse_variable_declaration();

		// Statement Parsing

		ptr<StatementNode> parse_statement();
		ptr<ReturnStatementNode> parse_return_statement();
		ptr<BlockStatementNode> parse_block_statement();
		ptr<ExprStatementNode> parse_expr_statement();
		ptr<IfStatementNode> parse_if_statement();

		// Expression Parsing

		ptr<ExpressionNode> parse_expression(uint8_t mininum_precedence = 0);
		ptr<ExpressionNode> parse_factor();
	private:
		int m_current_token_index{ 0 };
		ErrorHandler* m_error_handler{ nullptr };
		Token* m_current_token{ nullptr };

		bool m_error_occured{ false };

		TokenList m_tokens;
	};
}