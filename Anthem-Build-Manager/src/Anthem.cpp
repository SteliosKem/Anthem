#include <iostream>
#include <string>
#include "Utilities.h"
#include "Utilities/Utilities.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "CodeGeneration/CodeGenerator.h"
#include "CodeEmission/x86_GAS_Emitter.h"
#include "AIR/AIR.h"
#include "SemanticAnalyzer/SemanticAnalyzer.h"
#include "SemanticAnalyzer/TypeChecker.h"

enum class Platform {
	Windows,
	Linux
};

int main(int argc, char* argv[]) {
	if (argc < 2) {
		Anthem::log(Anthem::LogType::ERROR, "Expected filename");
		return -1;
	}

	std::vector<std::string> arguments(argv + 1, argv + argc);

	bool compile_for_windows = false;

	std::string source = Anthem::read_file(arguments[0]);
	if (std::find(arguments.begin(), arguments.end(), "-w") != arguments.end())
		compile_for_windows = true;

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
			Anthem::SemanticAnalyzer analyzer(&error_handler);
			analyzer.analyze_resolve(program_node);

			if (error_handler.has_errors())
				error_handler.print_errors();
			else {
				Anthem::TypeChecker type_checker(&error_handler);
				type_checker.check(program_node);

				if (error_handler.has_errors())
					error_handler.print_errors();
				else {
					std::cout << "Parse Tree for file: " << argv[1] << "\n";
					Anthem::Parser::pretty_print(program_node);
					std::cout << "\n";


					Anthem::AIRGenerator air_gen(&error_handler);
					Anthem::ptr<Anthem::AIRProgramNode> air_node = air_gen.generate(program_node, type_checker.get_symbols());
					std::cout << "\nAIR Output:\n";
					Anthem::AIRGenerator::pretty_print(air_node);

					Anthem::CodeGenerator code_gen(&error_handler, compile_for_windows);
					Anthem::ptr<Anthem::ASMProgramNode> asm_node = code_gen.generate(air_node);

					std::string output{ "" };
					Anthem::x86_GAS_Emitter emitter(compile_for_windows);
					emitter.emit(asm_node, output);
					std::cout << "\nAssembly Output for file: " << argv[1] << "\n" << output << "\n";

					std::filesystem::path path = argv[1];
					path = path.replace_extension("s");
					Anthem::write_file(path, output);

					std::string execute_gcc = "gcc " + path.string() + " -o " + path.string().substr(0, path.string().size() - 2);
					std::cout << execute_gcc << '\n';
					system(execute_gcc.c_str());
				}
			}
		}

		
	}

	return 0;
}