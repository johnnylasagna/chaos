#pragma once

#include <iostream>

#include "chaos/vm/cpu.hpp"
#include "chaos/vm/bus.hpp"
#include "chaos/vm/disk.hpp"

namespace Chaos {

enum class ChipError {
	UnknownSysCall
};

class Chip {
  public:
	Chip(Cpu& cpu_, Bus& bus_);

	std::expected<void, Error> run();
	std::expected<void, Error> copyFromDisk(Disk &disk, std::uint64_t diskAddress, std::uint64_t memoryAddress, std::size_t size);
	std::expected<void, Error> copyToDisk(Disk &disk, std::uint64_t memoryAddress, std::uint64_t diskAddress, std::size_t size);

	std::expected<void, Error> checkTrap(Disk &disk);

	std::expected<void, Error> printCpuState() const;

  private:
	Cpu& cpu;
	Bus& bus;

	std::expected<void, Error> handleSystemCall(Disk &disk);
};

} // namespace Chaos