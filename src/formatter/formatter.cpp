#include <iostream>
#include <expected>
#include <string>
#include <print>

#include "chaos/formatter/writer.hpp"
#include "chaos/common/errors.hpp"

// This program is used to write binary files into disk img

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "usage: formatter <file>\n";
		return 1;
	}

	const std::string path = argv[1];

	auto writeResult = Chaos::Writer::write(path, 0, "files/boot.chasm.bin");

	if (!writeResult) {
		Chaos::printError(writeResult.error());
		return 1;
	}

	writeResult = Chaos::Writer::write(path, 512, "files/kernel.chasm.bin");

	if (!writeResult) {
		Chaos::printError(writeResult.error());
		return 1;
	}

    std::println("Successfully wrote to file: {}", path);

	return 0;
}