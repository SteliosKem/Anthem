#include <iostream>
#include <string>
#include "Utilities.h"

namespace Anthem {
	int main(int argc, int argv[]) {
		if (argc < 2) {
			log(LogType::ERROR, "Expected filename");
			return -1;
		}

		return 0;
	}
}