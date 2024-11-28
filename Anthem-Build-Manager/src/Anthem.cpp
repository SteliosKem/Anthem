#include <iostream>
#include <string>
#include "Utilities.h"

namespace Anthem {
	
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		Anthem::log(Anthem::LogType::ERROR, "Expected filename");
		return -1;
	}
	std::string source = Anthem::read_file(argv[1]);
	Anthem::log(Anthem::LogType::INFO, source);

	return 0;
}