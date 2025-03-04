#pragma once
#include <filesystem>
#include <string>
#include <memory>
#include <variant>

namespace Anthem {
	template<typename T>
	using ptr = std::shared_ptr<T>;

	enum class VarType {
		I32
	};

	struct FunctionType {
		VarType return_type = VarType::I32;
		std::vector<VarType> parameters;
		bool is_external = false;
	};

	using Type = std::variant<VarType, FunctionType>;

	enum class UnaryOperation {
		NEGATE,
		COMPLEMENT,
		NOT,
		NONE
	};
	
	enum class VarFlag {
		Local,
		Global,
		Internal,
		External
	};

	enum class BinaryOperation {
		// -- Arithmetic --
		ADDITION,
		SUBTRACTION,
		DIVISION,
		MULTIPLICATION,
		REMAINDER,

		// -- Logical --
		AND,
		OR,

		// -- Relational --
		GREATER,
		LESS,
		GREATER_EQUAL,
		LESS_EQUAL,
		EQUAL,
		NOT_EQUAL,
		NONE
	};

	struct Position {
		std::filesystem::path src_file_path{ "" };

		// Token Start and End Index in source file
		int src_start_index{ 0 };
		int src_end_index{ 0 };

		// Line of Token in the source file
		int src_line{ 0 };
	};

	enum class LogType {
		INFO,
		ERROR,
		WARNING
	};

	// Log to Console
	void log(LogType type, const std::string& message);

	// Check if character is a digit
	bool is_digit(char character);
	// Check if character is a letter or underscore
	bool is_letter(char character);
	// Check if character is a letter or underscore or digit
	bool is_alphanumeric(char character);
}