#include "Lexer.h"

namespace Anthem {
	Lexer::Lexer(ErrorHandler* error_handler) : m_error_handler{ error_handler } {}

	void Lexer::advance(uint32_t times) {
		// If new index does not exceed the length of the source file then set the current character
		// to the one of the source string at the new index, else set it to a null character, indicating End of File
		for (int i = 0; i < times; i++) {
			if (m_current_source_index < m_source_code.size())
				m_current_char = m_source_code[m_current_source_index++];
			else
				m_current_char = '\0';
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
				advance();
				break;
			case '/':
				// Handle Single-Line comments
				if (peek() == '/') {
					while (current_character() != '\n' || current_character == '\0')
						advance();
					advance();
					return;
				}
				// Handle Multi-Line comments
				else if (peek() == '/*') {
					while (true) {
						if (current_character() == '\0')
							return;
						if (current_character() == '*' && peek() == '/') {
							advance(2);
							return;
						}
						advance();
					}
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

	}

	const std::vector<Token>& Lexer::analyze(const std::string& source) {
		m_current_source_index = -1;
		m_source_code = source;
		m_current_line = 0;
	}
}