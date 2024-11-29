#pragma once
#include <string>
#include "Utilities/Error.h"
#include "Token.h"

namespace Anthem {
	class Lexer {
	public:
		Lexer(ErrorHandler* handler);
		
		// Analyze whole source code
		const std::vector<Token>& analyze(const std::string& source);

		const std::vector<Token>& get_tokens();
	private:
		// Utility
		
		// Update to next character
		void advance();

		// Get a character current + <depth> index
		void peek(uint32_t depth = 0);

		// Lexical Analysis

		// Creates a token for current character
		Token lex();

		// Create a floating point or integer token
		Token make_number_token();
		Token make_string_token();

		// Create a keyword or identifier token
		Token make_name_token();
	private:
		std::string m_source_code;
		std::vector<Token> m_tokens;
		ErrorHandler* m_error_handler;
	};
}