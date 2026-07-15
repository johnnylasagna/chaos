#pragma once

#include <cstdint>
#include <expected>
#include <vector>

#include "chaos/common/errors.hpp"

#include <atomic>
#include <mutex>
#include <shared_mutex>

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
	std::expected<void, Error> copy(std::vector<uint8_t>& dest);

	std::atomic<bool> dirty{false};
	mutable std::shared_mutex mtx;

  private:
	std::vector<uint8_t> data;
};

template <typename T> std::expected<T, Error> Memory::read(uint64_t address) {
	size_t numBytes = sizeof(T);

	if (address > data.size() - numBytes)
		return std::unexpected(Error{MemoryError::ReadOutOfBounds, std::to_string(address)});

	T result = 0;

	std::shared_lock<std::shared_mutex> lock(mtx);

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

	std::unique_lock<std::shared_mutex> lock(mtx);

	while (numBytes--) {
		data[address + numBytes] = value & 0xFF;
		value >>= 8;
	}

	dirty.store(true);

	return {};
}

} // namespace Chaos