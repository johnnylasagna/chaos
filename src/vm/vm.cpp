#include <iostream>
#include <expected>
#include <limits>
#include <thread>

#include "chaos/vm/machine.hpp"

// This program is used to run a disk img in a vm

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "usage: vm <file>\n";
		return 1;
	}

	const std::string path = argv[1];

	Chaos::Machine machine(path);

	auto result = machine.init();

	if (!result) {
		Chaos::printError(result.error());
		return 1;
	}

	auto runResult = machine.run();

	if (!runResult) {
		Chaos::printError(runResult.error());
		return 2;
	}

	return 0;
}