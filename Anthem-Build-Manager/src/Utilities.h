#pragma once
#include <string>
#include <filesystem>

namespace Anthem {
	// Return as string the contents of a file
	std::string read_file(const std::filesystem::path& file_path);
	// Write String contents to File/Create it
	void write_file(const std::filesystem::path& file_path, const std::string& input);
}