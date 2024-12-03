#include <iostream>
#include <string>
#include "Utilities.h"
#include "Utilities/Utilities.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "CodeGeneration/CodeGenerator.h"
#include "CodeEmission/x86_GAS_Emitter.h"
#include "AIR/AIR.h"

namespace Anthem {
	
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		Anthem::log(Anthem::LogType::ERROR, "Expected filename");
		return -1;
	}
	std::string source = Anthem::read_file(argv[1]);
	
	// Lexing Phase

	Anthem::ErrorHandler error_handler{};
	Anthem::Lexer lexer(&error_handler);
	Anthem::TokenList tokens = lexer.analyze(source, argv[1]);
	if (error_handler.has_errors())
		error_handler.print_errors();
	else {
		std::cout << "Tokens for file: " << argv[1] << "\n";
		Anthem::Lexer::pretty_print(tokens);
		std::cout << '\n';

		// Parsing Phase

		Anthem::Parser parser(&error_handler);
		Anthem::ptr<Anthem::ProgramNode> program_node = parser.parse(tokens);
		if (error_handler.has_errors())
			error_handler.print_errors();
		else {
			std::cout << "Parse Tree for file: " << argv[1] << "\n";
			Anthem::Parser::pretty_print(program_node);
			std::cout << "\n";

			Anthem::AIRGenerator air_gen(&error_handler);
			Anthem::ptr<Anthem::AIRProgramNode> air_node = air_gen.generate(program_node);
			std::cout << "\nAIR Output:\n";
			Anthem::AIRGenerator::pretty_print(air_node);

			Anthem::CodeGenerator code_gen(&error_handler);
			Anthem::ptr<Anthem::ASMProgramNode> asm_node = code_gen.generate(air_node);

			std::string output{ "" };
			Anthem::x86_GAS_Emitter emitter{};
			emitter.emit(asm_node, output);
			std::cout << "\nAssembly Output for file: " << argv[1] << "\n" << output << "\n";

			std::filesystem::path path = argv[1];
			path = path.replace_extension("s");
			Anthem::write_file(path, output);
		}

		
	}

	return 0;
}