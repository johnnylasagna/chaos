#pragma once

#include <cstdint>
#include <expected>

#include "chaos/common/errors.hpp"
#include "chaos/vm/memory.hpp"
#include "chaos/vm/disk.hpp"

namespace Chaos {

enum class BusError { UnknownMemorySpace };

class Bus {
  public:
	Bus(Memory &ram_, Memory &video_, Memory &char_, Disk &disk_);

	template <typename T> std::expected<T, Error> read(std::uint64_t address);
	template <typename T> std::expected<void, Error> write(std::uint64_t address, T value);

	std::expected<void, Error> copyFromDisk(std::uint64_t diskAddress, std::uint64_t memoryAddress, std::size_t size);
	std::expected<void, Error> copyToDisk(std::uint64_t diskAddress, std::uint64_t memoryAddress, std::size_t size);

  private:
	Memory &ram;
	Memory &video;
	Memory &chars;
	Disk &disk;
};

template <typename T> std::expected<T, Error> Bus::read(std::uint64_t address) {
	if (address >= CharsOffset) {
		return chars.read<T>(address - CharsOffset);
	} else if (address >= VideoOffset) {
		return video.read<T>(address - VideoOffset);
	} else if (address >= RamOffset) {
		return ram.read<T>(address - RamOffset);
	}

	return std::unexpected(Error{BusError::UnknownMemorySpace, std::to_string(address)});
}
template <typename T> std::expected<void, Error> Bus::write(std::uint64_t address, T value) {
	if (address >= CharsOffset) {
		return chars.write<T>(address - CharsOffset, value);
	} else if (address >= VideoOffset) {
		return video.write<T>(address - VideoOffset, value);
	} else if (address >= RamOffset) {
		return ram.write<T>(address - RamOffset, value);
	}

	return std::unexpected(Error{BusError::UnknownMemorySpace, std::to_string(address)});
}

} // namespace Chaos