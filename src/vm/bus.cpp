#include "chaos/vm/bus.hpp"

namespace Chaos {

Bus::Bus(Memory &ram_, Memory &video_, Disk &disk_) : ram(ram_), video(video_), disk(disk_) {};

std::expected<void, Error> Bus::copyFromDisk(std::uint64_t diskAddress, std::uint64_t memoryAddress, std::size_t size) {
	for (std::size_t i = 0; i < size; ++i) {
		auto readResult = disk.read<std::uint8_t>(diskAddress + i);

		if (!readResult) {
			return std::unexpected(readResult.error());
		}

		auto writeResult = write<std::uint8_t>(memoryAddress + i, readResult.value());

		if (!writeResult) {
			return std::unexpected(writeResult.error());
		}
	}

	return {};
}

std::expected<void, Error> Bus::copyToDisk(std::uint64_t memoryAddress, std::uint64_t diskAddress, std::size_t size) {
	for (std::size_t i = 0; i < size; ++i) {
		auto readResult = read<std::uint8_t>(memoryAddress + i);

		if (!readResult) {
			return std::unexpected(readResult.error());
		}

		auto writeResult = disk.write<std::uint8_t>(diskAddress + i, readResult.value());

		if (!writeResult) {
			return std::unexpected(writeResult.error());
		}
	}

	return {};
}

} // namespace Chaos