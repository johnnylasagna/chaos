#pragma once

#include "chaos/vm/memory.hpp"
#include "chaos/vm/chip.hpp"
#include "chaos/vm/display.hpp"

#include <atomic>
#include <expected>

namespace Chaos {

class Machine {
  public:
	Machine(const std::string &path_);

	std::expected<void, Error> init();
    std::expected<void, Error> run();

  private:
	Chaos::Memory ram{Chaos::RamSize};
	Chaos::Memory video{Chaos::VideoSize};

	Chaos::Cpu cpu;
	Chaos::Bus bus{ram, video};

	Chaos::Chip chip{cpu, bus};
	Chaos::Disk disk;

	std::atomic<bool> running{false};
	std::string path;

    std::expected<void, Error> runDisplay();
    std::expected<void, Error> runChip();
};

} // namespace Chaos
