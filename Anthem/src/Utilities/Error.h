#pragma once
#include <string>
#include <filesystem>
#include <vector>

namespace Anthem {
	struct Error {
		// Error Info
		std::string message;

		// The path of the file the error occured in
		std::filesystem::path src_file_path;

		// Token start and end index in source file
		int src_start_index;
		int src_end_index;
		int src_line;
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
	private:
		std::vector<Error> m_errors;
	};
}