#pragma once
#include <filesystem>
#include <string>
#include <memory>
#include <variant>
#include <unordered_map>

namespace Anthem {
	template<typename T>
	using ptr = std::shared_ptr<T>;

	// Using 'Name' type in case it gets changed from std::string
	using Name = std::string;

	enum class VarFlag {
		Local,
		Global,
		Internal,
		External
	};

	enum class ReturnType {
		I32,
		I64,
		F32,
		F64,
		BOOL
	};

	struct VariableType {
		ReturnType return_type = ReturnType::I32;
		VarFlag flag = VarFlag::Local;
		int initializer = 0;
	};

	struct FunctionType {
		ReturnType return_type = ReturnType::I32;
		std::vector<ReturnType> parameters;
		bool is_external = false;
		VarFlag flag = VarFlag::Global;
	};

	using Type = std::variant<VariableType, FunctionType>;
	using SymbolTable = std::unordered_map<Name, Type>;

	enum class UnaryOperation {
		NEGATE,
		COMPLEMENT,
		NOT,
		NONE
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