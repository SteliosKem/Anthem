// Lexer.cpp
// Contains the Lexer Class method implementations
// Copyright (c) 2024-present, Stylianos Kementzetzidis

#include "Lexer.h"
#include <iostream>

namespace Anthem {
	Lexer::Lexer(ErrorHandler* error_handler) : m_error_handler{ error_handler } {}

	const std::vector<Token>& Lexer::get_tokens() {
		return m_tokens;
	}

	void Lexer::pretty_print() {
		pretty_print(m_tokens);
	}

	void Lexer::pretty_print(const std::vector<Token>& tokens) {
		// Pretty prints in this format: "<Line>     | <TokenType>     | <TokenValue>"
		std::cout << "Line:\t  TokenType:\t  Value:\n";
		
		// First line is set to an impossible number in order to be updated immediately at the start of the loop
		int current_line = -1;
		for (auto& token : tokens) {
			// Output 'Repeating' Symbol '|' if the line did not change since the previous token
			if (token.position.src_line != current_line) {
				current_line = token.position.src_line;
				std::cout << current_line;
			}
			else
				std::cout << '|';
			std::cout << "\t| " << token.type << "\t\t| " << token.value << "\n";
		}
	}

	void Lexer::advance(uint32_t times) {
		// If new index does not exceed the length of the source file then set the current character
		// to the one of the source string at the new index, else set it to a null character, indicating End of File
		for (uint32_t i = 0; i < times; i++) {
			m_current_source_index++;
			if (m_current_source_index < m_source_code.size())
				m_current_char = m_source_code[m_current_source_index];
			else
				m_current_char = '\0';
			m_current_position.src_start_index = m_current_source_index;
			m_current_position.src_end_index = m_current_source_index;
		}
	}

	char Lexer::peek(uint32_t depth) {
		// If current index + depth does not exceed the length of the source file then return the character
		// of the source string at the desired index, else set return a null character
		if (m_current_source_index + depth < m_source_code.size())
			return m_source_code[m_current_source_index + depth];
		else
			return '\0';
	}

	bool Lexer::match(char character) {
		if (peek() == character) {
			advance();
			return true;
		}
		return false;
	}

	char Lexer::current_character() {
		// Accessing current character through a function in case other functionality is needed here
		return m_current_char;
	}

	void Lexer::handle_whitespace() {
		// While current character is whitespace, skip it
		while (true) {
			switch (current_character())
			{
			// If character is space or tab just advance
			case ' ':
			case '\t':
				advance();
				break;
			// In case of a newline, increase the current line index
			case '\n':
				m_current_line++;
				m_current_position.src_line = m_current_line;
				advance();
				break;
			case '/':
				// Handle Single-Line comments
				if (peek() == '/') {
					while (current_character() != '\n' || current_character() == '\0')
						advance();
					advance();
					continue;
				}
				// Handle Multi-Line comments
				else if (peek() == '*') {
					while (true) {
						if (current_character() == '\0')
							return;
						if (current_character() == '*' && peek() == '/') {
							advance(2);
							break;
						}
						advance();
					}
					continue;
				}
				// If the slash turned out to not be a comment, stop checking for whitespace
				else
					return;
			default:
				return;
			}
		}
	}

	Token Lexer::lex() {
		// Skip over any whitespace
		handle_whitespace();

		if (is_digit(current_character())) return make_number_token();
		if (is_letter(current_character())) return make_name_token();

		Position position = m_current_position;

		// Position that has the end index increased, in case the Token will be of double characters
		Position next_position = position;
		next_position.src_end_index++;

		Token to_return;

		switch (current_character())
		{
		case '(':	advance(); return Token{ LEFT_PARENTHESIS, "(", position };
		case ')':	advance(); return Token{ RIGHT_PARENTHESIS, ")", position };
		case '{':	advance(); return Token{ LEFT_BRACE, "{", position };
		case '}':	advance(); return Token{ RIGHT_BRACE, "}", position };
		case '[':	advance(); return Token{ LEFT_BRACKET, "[", position };
		case ']':	advance(); return Token{ RIGHT_BRACKET, "]", position };
		case ';':	advance(); return Token{ SEMICOLON, ";", position };
		case ',':	advance(); return Token{ COMMA, ",", position };
		case '.':	advance(); return Token{ DOT, ".", position };
		case '^':	advance(); return Token{ CAP, "^", position };
		case '&':	advance(); return Token{ AMPERSAND, "&", position };
		case '~':	advance(); return Token{ TILDE, "~", position };
		case '%':	advance(); return Token{ PERCENT, "%", position };
		case ':':	advance(); return Token{ COLON, ":", position };
		// Check for double character tokens
		case '+':	to_return = match('=') ? Token{ PLUS_EQUAL, "+=", next_position } : Token{ PLUS, "+", position }; advance(); return to_return;
		case '-':	/* Match Arrow */ if (match('>')) { advance(); return Token{ARROW, "->", next_position}; }
					to_return = match('=') ? Token{ MINUS_EQUAL, "-=", next_position } : Token{ MINUS, "-", position }; advance(); return to_return;
		case '*':	to_return = match('=') ? Token{ STAR_EQUAL, "*=", next_position } : Token{ STAR, "*", position }; advance(); return to_return;
		case '/':	to_return = match('=') ? Token{ SLASH_EQUAL, "/=", next_position } : Token{ SLASH, "/", position }; advance(); return to_return;
		case '!':	to_return = match('=') ? Token{ BANG_EQUAL, "!=", next_position } : Token{ BANG, "!", position }; advance(); return to_return;
		case '=':	to_return = match('=') ? Token{ EQUAL_EQUAL, "==", next_position } : Token{ EQUAL, "=", position }; advance(); return to_return;
		case '<':	to_return = match('=') ? Token{ LESS_EQUAL, "<=", next_position } : Token{ LESS, "<", position }; advance(); return to_return;
		case '>':	to_return = match('=') ? Token{ GREATER_EQUAL, ">=", next_position } : Token{ GREATER, ">", position }; advance(); return to_return;
		case '"':	return make_string_token();
		case '\0':	return Token{ SPECIAL_EOF, "EOF", position };
		default:
			break;
		}

		// If this part of the code is reached, then the current character does not match any known one, report an error
		m_error_handler->report_error(Error{ std::format("Unkown Character '{0}'", current_character()), position });
		return Token{SPECIAL_ERROR, "", position};
	}

	Token Lexer::make_number_token() {
		std::string number_value{ "" };
		bool is_floating_point{ false };

		// Store position for start index
		Position position = m_current_position;

		// Make a number from all digits next to eachother in the string
		while (is_digit(current_character()) || current_character() == '.') {
			if (current_character() == '.') {
				// If there was already a dot in this number, report an unexpected dot error
				if (is_floating_point) {
					m_error_handler->report_error(Error{ "Unexpected '.'", m_current_position });
					return Token{SPECIAL_ERROR, "Unexpected '.'", m_current_position };
				}
				else {
					is_floating_point = true;
					number_value += current_character();
					advance();
				}
			}
			else {
				number_value += current_character();
				advance();
			}
		}

		position.src_end_index = m_current_source_index - 1;

		return is_floating_point ? Token{ TYPE_F32, number_value, position } : Token{ TYPE_I32, number_value, position };
	}

	Token Lexer::make_name_token() {
		std::string name_string{ "" };

		// Store position for start index
		Position position = m_current_position;

		// Make name from all characters next to eachother in the string
		while (is_alphanumeric(current_character())) {
			name_string += current_character();
			advance();
		}

		position.src_end_index = m_current_source_index;

		TokenType keyword_type = get_keyword(name_string);
		return keyword_type == NO_TYPE ? Token{ IDENTIFIER, name_string, position } : Token{ keyword_type, name_string, position};
	}

	Token Lexer::make_string_token() {
		// TO-DO
		return Token{};
	}

	const std::vector<Token>& Lexer::analyze(const std::string& source, const std::filesystem::path& file_path) {
		// Reset internal values
		m_current_source_index = -1;
		m_source_code = source;
		m_current_line = 0;
		m_current_position.src_file_path = file_path;
		m_tokens.clear();

		advance();

		// Lex until End of File
		while (current_character() != '\0' && !m_error_handler->has_errors())
			m_tokens.push_back(lex());
		
		m_tokens.push_back(Token{SPECIAL_EOF, "EOF", m_current_position});

		return m_tokens;
	}
}