#pragma once
#include <string>
#include <filesystem>
#include <vector>
#include "Utilities.h"

namespace Anthem {
	struct Error {
		// Error Info
		std::string message;

		// Position info for the token that caused the error
		Position token_position{};
	};

	// Manages a list of errors for uniform error handling throughout all phases of compilation
	class ErrorHandler {
	public:
		ErrorHandler() = default;

		const std::vector<Error>& get_errors() const { return m_errors; }

		// Add error to the list
		void report_error(const Error& error);

		// Log all errors to the console
		void print_errors();

		// Return if there were any errors throughout compilation
		bool has_errors();
	private:
		std::vector<Error> m_errors;
	};
}