#include "chaos/hex/viewer.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <iostream>

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: chaos-hex <file>\n";
		return 1;
	}

	Chaos::Viewer viewer;

	auto result = viewer.open(argv[1]);
	if (!result) {
		Chaos::printError(result.error());
		return 2;
	}

	size_t offset = 0;

	auto screen = ftxui::ScreenInteractive::TerminalOutput();

	ftxui::Component component = ftxui::Renderer([&] { return viewer.render(offset, 32) | ftxui::border; });

	component |= ftxui::CatchEvent([&](ftxui::Event event) {
		if (event == ftxui::Event::ArrowDown) {
			offset += 16;
			return true;
		}

		if (event == ftxui::Event::ArrowUp) {
			if (offset >= 16)
				offset -= 16;
			return true;
		}

		if (event == ftxui::Event::PageDown) {
			offset += 32 * 16;
			return true;
		}

		if (event == ftxui::Event::PageUp) {
			offset = offset >= 32 * 16 ? offset - 32 * 16 : 0;
			return true;
		}

		if (event == ftxui::Event::Character('q') || event == ftxui::Event::Escape) {
			screen.Exit();
			return true;
		}

		return false;
	});

	screen.Loop(component);

	return 0;
}