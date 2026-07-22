#pragma once

#include "chaos/vm/memory.hpp"
#include "chaos/vm/chip.hpp"
#include "chaos/vm/display.hpp"
#include "chaos/vm/debug.hpp"

#include <atomic>
#include <expected>
#include <shared_mutex>

namespace Chaos {

class Machine {
  public:
	Machine(const std::string &path_);

	std::expected<void, Error> init();
	std::expected<void, Error> run();

  private:
	Chaos::Memory ram{Chaos::RamSize};
	Chaos::Memory video{Chaos::VideoSize};
	Chaos::Memory chars{Chaos::CharsSize};
	Chaos::Disk disk;

	Chaos::Cpu cpu;
	Chaos::Bus bus{ram, video, chars, disk};

	Chaos::Chip chip{cpu, bus};

	std::shared_mutex instructionMutex;
	Chaos::Debug debug{cpu, ram, instructionMutex};

	std::atomic<bool> running{false};
	std::string path;

	std::expected<void, Error> runDisplay();
	std::expected<void, Error> runChip();
	std::expected<void, Error> runDebug();
};

} // namespace Chaos
