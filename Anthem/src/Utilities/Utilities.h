#pragma once
#include <string>

namespace Anthem {
	enum class LogType {
		INFO,
		ERROR,
		WARNING
	};

	// Log to Console
	void log(LogType type, const std::string& message);
}