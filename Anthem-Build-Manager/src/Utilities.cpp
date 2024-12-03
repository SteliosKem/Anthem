#include "Utilities.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace Anthem {
	std::string read_file(const std::filesystem::path& file_path) {
		// Read the file
		std::ifstream file(file_path);

		// Store file content to stringstream buffer
		std::stringstream buffer;
		buffer << file.rdbuf();

		return buffer.str();
	}

	void write_file(const std::filesystem::path& file_path, const std::string& input) {
		// Open the file
		std::ofstream file(file_path);
		file << input;
		file.close();
	}
}