#pragma once
#include <string>

namespace Anthem {
	struct Position {
		std::filesystem::path src_file_path{ "" };

		// Token Start and End Index in source file
		int src_start_index{ 0 };
		int src_end_index{ 0 };

		// Line of Token in the source file
		int src_line{ 0 };
	};

	enum class LogType {
		INFO,
		ERROR,
		WARNING
	};

	// Log to Console
	void log(LogType type, const std::string& message);
}