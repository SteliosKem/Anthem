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

	bool is_digit(char character) {
		return character >= '0' && character <= '9';
	}

	bool is_letter(char character) {
		return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') || character == '_';
	}

	bool is_alphanumeric(char character) {
		return is_digit(character) || is_letter(character);
	}

}