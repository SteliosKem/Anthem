#include "Utilities.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace Anthem {
	void log(LogType type, const std::string& message) {
		switch (type)
		{
		case Anthem::LogType::INFO:
			std::cout << "INFO: " << message << std::endl;
			break;
		case Anthem::LogType::ERROR:
			std::cout << "ERROR: " << message << std::endl;
			break;
		case Anthem::LogType::WARNING:
			std::cout << "WARNING: " << message << std::endl;
			break;
		default:
			break;
		}
	}

	std::string read_file(const std::filesystem::path& file_path) {
		// Read the file
		std::ifstream file(file_path);

		// Store file content to stringstream buffer
		std::stringstream buffer;
		buffer << file.rdbuf();

		return buffer.str();
	}
}