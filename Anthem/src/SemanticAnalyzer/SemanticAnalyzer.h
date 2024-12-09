#pragma once
#include "Utilities/Error.h"
#include "Parser/Parser.h"
#include <unordered_map>

namespace Anthem {
	class SemanticAnalyzer {
	public:
		SemanticAnalyzer(ErrorHandler* error_handler);

		void analyze_resolve(ptr<ProgramNode> program_node);
	private:
		// -- Utility --

		void report_error(const std::string& error_msg, const Token& token);

		// -- Analysis --

		void analyze_declaration(ptr<DeclarationNode> declaration_node);

		void analyze_statement(ptr<StatementNode> statement);
		void analyze_expression(ptr<ExpressionNode> expression);

		// Generate unique name
		Name make_unique(const Name& name);

		// Map for local variables
		std::unordered_map<Name, Name> m_local_map;
		uint64_t m_unique_counter{ 0 };
		ErrorHandler* m_error_handler;
	};
}