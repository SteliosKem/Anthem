#pragma once
#include <string>
#include <filesystem>

namespace Anthem {
	enum class LogType {
		INFO,
		ERROR,
		WARNING
	};

	// Log to Console
	void log(LogType type, const std::string& message);

	// Return as string the contents of a file
	std::string read_file(const std::filesystem::path& file_path);
}