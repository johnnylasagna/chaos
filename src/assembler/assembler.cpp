#include "chaos/assembler/translator.hpp"
#include "chaos/common/errors.hpp"

#include <iostream>

// This program is used to compile .chasm files into .chasm.bin files

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "usage: assembler <file>\n"; 
		return 1;
	}

	const std::string path = argv[1];

	auto result = Chaos::Assembler::assemble(path);

	if (!result) {
		Chaos::printError(result.error());
		return 1;
	}

	std::cout << "Compilation successful\n";

	return 0;
}