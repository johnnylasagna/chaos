#include "chaos/vm/debug.hpp"

#include <format>

namespace Chaos {

Debug::Debug(Cpu &cpu_, Memory &ram_, std::shared_mutex &instructionMutex_) : cpu(cpu_), ram(ram_), instructionMutex(instructionMutex_) {}

void Debug::init() {
	using namespace ftxui;

	cpuTab = Renderer([&] {
		decltype(cpu.getState()) state;

		{
			std::shared_lock lock(instructionMutex);
			state = cpu.getState();
		}

		auto &[registers, flags] = state;

		Elements regLines;

		regLines.push_back(hbox({
		    text(std::format("{:<2}", "Id")) | color(Color::GrayDark),
		    text(" │ "),
		    text(std::format("{:<10}", "Register")) | color(Color::YellowLight),
		    text(" │ "),
		    text(std::format("{:18}","Value")) | color(Color::MagentaLight),
			text(" │ "),
		    text("Value (Decimal)") | color(Color::MagentaLight),
		}));

		regLines.push_back(separator());

		for (int i = 0; i < RegisterCount; ++i) {
			auto name = shortEnumName<Register>(static_cast<Register>(i));

			regLines.push_back(hbox({
			    text(std::format("{:02}", i)) | color(Color::GrayDark),

			    text(" │ "),

			    text(std::format("{:<10}", name)) | color(Color::YellowLight),

			    text(" │ "),

			    text(std::format("0x{:016X}", registers[i])) | color(Color::White),

			    text(" │ "),

			    text(std::format("{}", registers[i])) | color(Color::White),
			}));
		}

		Elements flagParts;

		for (int i = 0; i < FlagCount; ++i) {
			auto name = shortEnumName<Flag>(static_cast<Flag>(i));

			if (i != 0)
				flagParts.push_back(text("    "));

			flagParts.push_back(text(std::format("{:<8}", name)) | color(Color::CyanLight));

			flagParts.push_back(text(": "));

			flagParts.push_back(text(flags[i] ? "ON " : "OFF") | bold | color(flags[i] ? Color::GreenLight : Color::GrayLight));
		}

		return vbox({
		    window(text(" Registers"), vbox(std::move(regLines))),
		    window(text(" Flags"), hbox(std::move(flagParts))),
		});
	});

	container = Container::Tab({cpuTab}, &tab);
}

void Debug::run() { screen.Loop(container); }

void Debug::refresh() { screen.PostEvent(ftxui::Event::Custom); }

void Debug::exit() { screen.Exit(); }

} // namespace Chaos