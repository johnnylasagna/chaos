#pragma once

#include <cstdint>
#include <expected>
#include <vector>

#include "chaos/common/errors.hpp"

namespace Chaos {

inline constexpr size_t RamOffset = 0;
inline constexpr size_t RamSize = 4 * 1024; // 4 KB
inline constexpr size_t VideoOffset = RamSize;
inline constexpr size_t VideoWidth = 800;
inline constexpr size_t VideoHeight = 600;
inline constexpr size_t VideoSize = VideoWidth * VideoHeight; // 467 KB

enum class MemoryError { ReadOutOfBounds, WriteOutOfBounds };

class Memory {
  public:
	Memory(const size_t size);

	template <typename T> std::expected<T, Error> read(uint64_t address);
	template <typename T> std::expected<void, Error> write(uint64_t address, T value);

  private:
	std::vector<uint8_t> data;
};

template <typename T> std::expected<T, Error> Memory::read(uint64_t address) {
	size_t numBytes = sizeof(T);

	if (address > data.size() - numBytes)
		return std::unexpected(Error{MemoryError::ReadOutOfBounds, std::to_string(address)});

	T result = 0;

	while (numBytes--) {
		result <<= 8;
		result |= data[address++];
	}

	return result;
}

template <typename T> std::expected<void, Error> Memory::write(uint64_t address, T value) {
	size_t numBytes = sizeof(T);

	if (address > data.size() - numBytes)
		return std::unexpected(Error{MemoryError::WriteOutOfBounds, std::to_string(address)});

	while (numBytes--) {
		data[address + numBytes] = value & 0xFF;
		value >>= 8;
	}

	return {};
}

} // namespace Chaos