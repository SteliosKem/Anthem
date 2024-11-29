#include "Error.h"
#include "Utilities.h"

namespace Anthem {
	void ErrorHandler::report_error(const Error& error) {
		m_errors.push_back(error);
	}

	void ErrorHandler::print_errors() {
		for (auto& error : m_errors) {
			log(LogType::ERROR, error.message + " at line: " + std::to_string(error.src_line));
		}
	}
}