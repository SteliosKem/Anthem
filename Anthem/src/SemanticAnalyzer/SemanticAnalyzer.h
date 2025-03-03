// SemanticAnalyzer.h
// Contains the SemanticAnalyzer Class definition
// Copyright (c) 2024-present, Stylianos Kementzetzidis

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

		using VarMap = std::unordered_map<Name, Name>;

		void report_error(const std::string& error_msg, const Token& token);

		// Get current variable/name map
		VarMap& current_map();

		// Start new loop
		uint64_t new_loop();

		// Get current loop
		uint64_t current_loop();

		// -- Analysis --

		// Add declarations to map / Handle multiple declarations
		void save_declaration(ptr<DeclarationNode> declaration_node);

		void analyze_declaration(ptr<DeclarationNode> declaration_node);
		void analyze_statement(ptr<StatementNode> statement);
		void analyze_expression(ptr<ExpressionNode> expression);

		// Generate unique name
		Name make_unique(const Name& name);

		// Maps for local variables
		std::vector<VarMap> m_local_map_stack;
		VarMap m_global_map;

		// Stack for loop ids;
		std::vector<uint64_t> m_loop_stack;
		uint64_t m_loop_counter{ 0 };

		uint64_t m_unique_counter{ 0 };
		ErrorHandler* m_error_handler;
	};
}