// Lexer.h
// Contains the Lexer Class definition
// Copyright (c) 2024-present, Stylianos Kementzetzidis

#pragma once
#include <string>
#include "Utilities/Error.h"
#include "Token.h"

namespace Anthem {
	class Lexer {
	public:
		Lexer(ErrorHandler* handler);
		
		// Tokenize whole source code
		const std::vector<Token>& analyze(const std::string& source, const std::filesystem::path& file_path = "");

		// Print the list of tokens to the console (in an almost readable way)
		void pretty_print();
		static void pretty_print(const std::vector<Token>& tokens);

		const std::vector<Token>& get_tokens();
	private:


		// -- Utility --
		

		// Move to next character
		void advance(uint32_t times = 1);

		// Get a character at (current + <depth>) index
		char peek(uint32_t depth = 1);

		// Return true and advance if next character matches the given one
		bool match(char character);

		// Returns the character of the source string at the current index
		char current_character();


		// -- Lexical Analysis --


		// Skips over any whitespace characters
		void handle_whitespace();

		// Returns a token depending on the current character(s)
		Token lex();

		// Create a floating point or integer literal token
		Token make_number_token();

		// Create a string literal token
		Token make_string_token();

		// Create a keyword or identifier token
		Token make_name_token();

	private:
		int m_current_source_index{ -1 };
		char m_current_char{ '\0' };
		uint32_t m_current_line{ 0 };

		Position m_current_position;
		
		std::filesystem::path m_file_path{ "" };
		std::string m_source_code{ "" };
		std::vector<Token> m_tokens;
		ErrorHandler* m_error_handler{ nullptr };
	};
}