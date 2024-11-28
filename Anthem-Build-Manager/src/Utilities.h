#pragma once
#include <string>
#include <filesystem>

namespace Anthem {
	// Return as string the contents of a file
	std::string read_file(const std::filesystem::path& file_path);
}