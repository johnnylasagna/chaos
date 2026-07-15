#pragma once

#include <iostream>

#include "chaos/vm/cpu.hpp"
#include "chaos/vm/bus.hpp"
#include "chaos/vm/disk.hpp"

namespace Chaos {

class Chip {
  public:
	Chip(Cpu& cpu_, Bus& bus_);

	std::expected<void, Error> run();

	std::expected<void, Error> printCpuState() const;

  private:
	Cpu& cpu;
	Bus& bus;
};

} // namespace Chaos