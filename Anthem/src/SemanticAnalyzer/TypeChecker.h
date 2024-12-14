#pragma once
#include <unordered_map>
#include "Utilities/Utilities.h"
#include "Utilities/Error.h"
#include <Parser/ASTNodes.h>

namespace Anthem {
	using SymbolTable = std::unordered_map<Name, Type>;

	class TypeChecker {
	public:
		TypeChecker(ErrorHandler* error_handler);

		void check(ptr<ProgramNode> program);

	private:
		void check_declaration(ptr<DeclarationNode> declaration);
		void check_statement(ptr<StatementNode> statement);
		void check_expression(ptr<ExpressionNode> expression);

		void track_function(ptr<DeclarationNode> declaration);
	private:
		SymbolTable m_symbol_table;

		ErrorHandler* m_error_handler;
	};
}