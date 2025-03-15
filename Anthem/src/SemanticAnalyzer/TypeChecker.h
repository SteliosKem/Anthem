// TypeChecker.h
// Contains the TypeChecker Class definition
// Copyright (c) 2024-present, Stylianos Kementzetzidis

#pragma once
#include <unordered_map>
#include "Utilities/Utilities.h"
#include "Utilities/Error.h"
#include <Parser/ASTNodes.h>

namespace Anthem {
	class TypeChecker {
	public:
		TypeChecker(ErrorHandler* error_handler);

		// Type Check an AST
		void check(ptr<ProgramNode> program);

		SymbolTable& get_symbols() { return m_symbol_table; }

	private:
		void check_declaration(ptr<DeclarationNode> declaration);
		void check_statement(ptr<StatementNode> statement);
		void check_expression(ptr<ExpressionNode> expression);

		// Add function to the symbol table
		void track_function(ptr<DeclarationNode> declaration);
	private:
		SymbolTable m_symbol_table;

		ErrorHandler* m_error_handler;
	};
}