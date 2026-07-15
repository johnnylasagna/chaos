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

	auto copyResult = chip.copyFromDisk(disk, 0, 0, 512);

	if (!copyResult) {
		return std::unexpected(copyResult.error());
	}

	return {};
}

std::expected<void, Error> Machine::run() {
	running.store(true);

	auto displayFuture = std::async(std::launch::async, &Machine::runDisplay, this);
	auto chipFuture = std::async(std::launch::async, &Machine::runChip, this);

	auto displayResult = displayFuture.get();
	auto chipResult = chipFuture.get();

	if (!displayResult) {
		return std::unexpected(displayResult.error());
	}

	if (!chipResult) {
		return std::unexpected(chipResult.error());
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
		auto runResult = chip.run();

		if (!runResult) {
			running.store(false);
			return std::unexpected(runResult.error());
		}

		auto trapResult = chip.checkTrap(disk);

		if (!trapResult) {
			running.store(false);
			return std::unexpected(trapResult.error());
		}
	}

	return {};
}

} // namespace Chaos