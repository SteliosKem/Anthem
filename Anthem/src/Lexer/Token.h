#pragma once
#include <string>
#include <filesystem>
#include "Utilities/Utilities.h"

namespace Anthem {
	enum TokenType {
		// Single Character Tokens
		// --

			// Grouped Tokens
			LEFT_PARENTHESIS,
			RIGHT_PARENTHESIS,

			LEFT_BRACE,
			RIGHT_BRACE,

			LEFT_BRACKET,
			RIGHT_BRACKET,

			// Arithmetic
			PLUS,
			MINUS,
			STAR,
			SLASH,
			CAP,
			PERCENT,

			// Logic
			AMPERSAND,
			TILDE,
			PIPE,
			BANG,

			// Comparison
			GREATER,
			LESS,

			DOT,
			COMMA,
			SEMICOLON,

			// Assignment
			EQUAL,
		// --

		// Double Character Tokens

		// --
			// Comparison
			BANG_EQUAL,
			EQUAL_EQUAL,
			GREATER_EQUAL,
			LESS_EQUAL,

			// Compound Assignment
			PLUS_EQUAL,
			MINUS_EQUAL,
			STAR_EQUAL,
			SLASH_EQUAL,

			ARROW,
		// --

		// Keywords

		// --
			AND,
			OR,
			TRUE,
			FALSE,
			IF,
			ELSE,
			WHILE,
			LOOP,
			FOR,
			RETURN,
			FUNCTION,
			DO,
			BREAK,
			CONTINUE,
			CLASS,
			THIS,
			ENUM,
			KEY_I8,
			KEY_I16,
			KEY_I32,
			KEY_I64,
			KEY_F32,
			KEY_F64,
			KEY_BOOL,
		// --

		// Types

		// --
			TYPE_I8,
			TYPE_I16,
			TYPE_I32,
			TYPE_I64,
			TYPE_F32,
			TYPE_F64,
			TYPE_BOOL
		// --
	};

	struct Token {
		TokenType type;
		std::string value;
		
		Position position;
	};
}