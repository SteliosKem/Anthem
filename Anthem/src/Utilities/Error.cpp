#include "Error.h"
#include "Utilities.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace Anthem {
	void ErrorHandler::report_error(const Error& error) {
		m_errors.push_back(error);
	}

	void ErrorHandler::print_errors() {
		for (auto& error : m_errors) {
			// -- Read file contents --
			std::ifstream file(error.token_position.src_file_path);

			// Store file content to stringstream buffer
			std::stringstream buffer;
			buffer << file.rdbuf();

			std::string source = buffer.str();

			// -- File content substring to output comprehensive Error Message --
			const int maximum_line_padding_characters = 30;
			int leading_index = 0; // <-- How many characters will get included before the problematic Token
			int after_index = 0; // <-- After the problematic Token

			// Begin loop at the start of the error Token
			for (int i = error.token_position.src_start_index; i > 0; i--) {
				if (source[i] == '\n')
					break;
				if (leading_index >= maximum_line_padding_characters)
					break;
				leading_index++;
			}

			// Begin loop at the end of the error Token
			for (int i = error.token_position.src_end_index; i < source.size(); i++) {
				if (source[i] == '\n')
					break;
				if (after_index >= maximum_line_padding_characters)
					break;
				after_index++;
			}

			int start_pos = error.token_position.src_start_index - leading_index;
			int end_pos = error.token_position.src_end_index + after_index;

			std::string error_line = (leading_index == maximum_line_padding_characters ? "..." : "") 
				+ source.substr(start_pos, end_pos - start_pos) 
				+ (after_index == maximum_line_padding_characters ? "..." : "");

			std::string line_info = std::format("Line {0}: ", error.token_position.src_line);
																									 // 3 is the size of "..."
			int padding_amount = line_info.size() + (leading_index == maximum_line_padding_characters ? 3 + leading_index : leading_index) - 1;
			std::string arrows;
			for (int i = 0; i < padding_amount; i++)
				arrows += " ";
			int arrow_number = error.token_position.src_end_index - error.token_position.src_start_index + 1;
			for (int i = 0; i < arrow_number; i++)
				arrows += "^";
			

			log(LogType::ERROR, error.message + " at file: '" + 
				error.token_position.src_file_path.filename().string() + "', line: " + std::to_string(error.token_position.src_line));
			std::cout << line_info << error_line << '\n' << arrows << "\n\n";
		}
	}

	bool ErrorHandler::has_errors() {
		return !m_errors.empty();
	}
}