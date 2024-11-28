#include "Utilities.h"
#include <iostream>

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
}