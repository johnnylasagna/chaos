#pragma once

#include "chaos/vm/cpu.hpp"
#include "chaos/vm/memory.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <shared_mutex>

namespace Chaos {

class Debug {
  public:
	Debug(Cpu &cpu_, Memory &ram_, std::shared_mutex& instructionMutex_);
	void init();
	void run();
	void refresh();
	void exit();

  private:
	ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::TerminalOutput();
	ftxui::Component cpuTab;
	ftxui::Component container;

	Cpu &cpu;
	Memory &ram;
    std::shared_mutex& instructionMutex;

	int tab{};
};

} // namespace Chaos