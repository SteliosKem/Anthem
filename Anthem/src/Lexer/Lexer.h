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
		// -- Utility --
		
		// Update to next character
		void advance(uint32_t times = 1);

		// Get a character current + <depth> index
		char peek(uint32_t depth = 0);

		// Returns the character of the source string at the current index
		char current_character();

		// -- Lexical Analysis --

		// Skips over any whitespace characters
		void handle_whitespace();

		// Creates a token for current character
		Token lex();

		// Create a floating point or integer token
		Token make_number_token();
		Token make_string_token();

		// Create a keyword or identifier token
		Token make_name_token();
	private:
		int m_current_source_index{ -1 };
		char m_current_char = '\0';
		uint32_t m_current_line = 0;

		std::string m_source_code{ "" };
		std::vector<Token> m_tokens;
		ErrorHandler* m_error_handler{ nullptr };
	};
}