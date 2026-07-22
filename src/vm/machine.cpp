#include "chaos/vm/machine.hpp"

#include "chaos/vm/img.hpp"

#include <future>

namespace Chaos {

Machine::Machine(const std::string &path_) : path(path_) {}

std::expected<void, Error> Machine::init() {
	auto loadResult = Chaos::Img::load(disk, path);

	if (!loadResult) {
		return std::unexpected(loadResult.error());
	}

	auto copyResult = bus.copyFromDisk(0, 0, 512);

	if (!copyResult) {
		return std::unexpected(copyResult.error());
	}

	copyResult = bus.copyFromDisk(1024, CharsOffset, CharsSize);

	if (!copyResult) {
		return std::unexpected(copyResult.error());
	}

	debug.init();

	return {};
}

std::expected<void, Error> Machine::run() {
	running.store(true);

	auto displayFuture = std::async(std::launch::async, &Machine::runDisplay, this);
	auto chipFuture = std::async(std::launch::async, &Machine::runChip, this);
	auto debugFuture = std::async(std::launch::async, &Machine::runDebug, this);

	auto displayResult = displayFuture.get();
	auto chipResult = chipFuture.get();
	auto debugResult = debugFuture.get();

	if (!displayResult) {
		return std::unexpected(displayResult.error());
	}

	if (!chipResult) {
		return std::unexpected(chipResult.error());
	}

	if (!debugResult) {
		return std::unexpected(debugResult.error());
	}

	auto saveResult = Chaos::Img::save(disk, path);

	if (!saveResult) {
		return std::unexpected(saveResult.error());
	}

	return {};
}

std::expected<void, Error> Machine::runDisplay() {
	Chaos::Display display{video};

	while (running.load() && !WindowShouldClose()) {
		display.show();
	}

	running.store(false);

	return {};
}

std::expected<void, Error> Machine::runChip() {
	while (running.load()) {
		auto runResult = [&] {
			std::unique_lock lock(instructionMutex);
			return chip.run();
		}();

		if (!runResult) {
			running.store(false);
			return std::unexpected(runResult.error());
		}

		debug.refresh();
	}

	return {};
}

std::expected<void, Error> Machine::runDebug() {
	while (running.load()) {
		debug.run();
	}

	debug.exit();

	return {};
}

} // namespace Chaos