#pragma once
#include "Utilities/Error.h"
#include "ASMProgramStruct.h"
#include "Parser/Parser.h"

namespace Anthem {
	class CodeGenerator {
	public:
		CodeGenerator(ErrorHandler* error_handler);

		ptr<ASMProgramNode> generate(ptr<ProgramNode> program);
	private:

	};
}