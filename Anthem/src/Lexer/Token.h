#pragma once
#include <string>
#include <filesystem>
#include "Utilities/Utilities.h"
#include <unordered_map>

namespace Anthem {
	
	enum TokenType {
		// -- Single Character Tokens --
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

		// -- Double Character Tokens --
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

		// -- Keywords --
			LET,
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

		// -- Types --
			IDENTIFIER,
			TYPE_I8,
			TYPE_I16,
			TYPE_I32,
			TYPE_I64,
			TYPE_F32,
			TYPE_F64,
			TYPE_BOOL,
		// --

		// -- Special --
			SPECIAL_EOF,
			SPECIAL_ERROR,
			NO_TYPE,
	};

	struct Token {
		TokenType type;
		std::string value;
		
		Position position;
	};

	const std::unordered_map<std::string, TokenType> keyword_map = {
		{ "let"		, LET	},
		{ "and"		, AND	},
		{ "or"		, OR	},
		{ "true"	, TRUE	},
		{ "false"	, FALSE	},
		{ "if"		, IF	},
		{ "else"	, ELSE	},
		{ "while"	, WHILE	},
		{ "loop"	, LOOP	},
		{ "for"		, FOR	},
		{ "return"	, RETURN	},
		{ "fn"		, FUNCTION	},
		{ "do"		, DO	},
		{ "break"	, BREAK	},
		{ "continue", CONTINUE	},
		{ "class"	, CLASS	},
		{ "this"	, THIS	},
		{ "enum"	, ENUM	},
		{ "i8"		, KEY_I8	},
		{ "i16"		, KEY_I16	},
		{ "i32"		, KEY_I32	},
		{ "i64"		, KEY_I64	},
		{ "f32"		, KEY_F32	},
		{ "f64"		, KEY_F64	},
		{ "bool"	, KEY_BOOL	},
	};

	inline TokenType get_keyword(const std::string& name) {
		if (keyword_map.find(name) != keyword_map.end())
			return keyword_map.at(name);
		return NO_TYPE;
	}
}