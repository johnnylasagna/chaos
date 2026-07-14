#include <iostream>
#include <expected>
#include <limits>

#include "chaos/vm/chip.hpp"
#include "chaos/vm/memory.hpp"
#include "chaos/vm/img.hpp"
#include "chaos/common/errors.hpp"
#include "chaos/vm/display.hpp"

// This program is used to run a disk img in a vm

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "usage: vm <file>\n";
		return 1;
	}

	const std::string path = argv[1];

	Chaos::Memory ram(Chaos::RamSize);
	Chaos::Memory video(Chaos::VideoSize);

	Chaos::Cpu cpu;
	Chaos::Bus bus(ram, video);

	Chaos::Chip chip(cpu, bus);
	Chaos::Disk disk;

	Chaos::Display display(video);

	auto loadResult = Chaos::Img::load(disk, path);

	if (!loadResult) {
		Chaos::printError(loadResult.error());
		return 1;
	}

	auto copyResult = chip.copyFromDisk(disk, 0, 0, 512);

	if (!copyResult) {
		Chaos::printError(copyResult.error());
		return 1;
	}

	while (true) {
		auto runResult = chip.run();

		if (!runResult) {
			Chaos::printError(runResult.error());
			return 1;
		}

		auto trapResult = chip.checkTrap(disk);

		if (!trapResult) {
			Chaos::printError(trapResult.error());
		}

		display.show();
	}

	display.close();

	CloseWindow();

	auto saveResult = Chaos::Img::save(disk, path);

	if (!saveResult) {
		Chaos::printError(saveResult.error());
		return 1;
	}

	return 0;
}