#pragma once

#include <expected>
#include <cstdint>
#include <vector>

#include "chaos/common/errors.hpp"

namespace Chaos {

inline constexpr size_t DiskSize = 4 * 1024 * 1024; // 4 MB

enum class DiskError { ReadOutOfBounds, WriteOutOfBounds };

class Disk {
  public:
	Disk();

	template <typename T> std::expected<T, Error> read(uint64_t address);
	template <typename T> std::expected<void, Error> write(uint64_t address, T value);

  private:
	std::vector<uint64_t> data;
};

template <typename T> std::expected<T, Error> Disk::read(uint64_t address) {
	size_t numBytes = sizeof(T);

	if (address > data.size() - numBytes)
		return std::unexpected(Error{DiskError::ReadOutOfBounds, std::to_string(address)});

	T result = 0;

	while (numBytes--) {
		result <<= 8;
		result |= data[address++];
	}

	return result;
}

template <typename T> std::expected<void, Error> Disk::write(uint64_t address, T value) {
	size_t numBytes = sizeof(T);

	if (address > data.size() - numBytes)
		return std::unexpected(Error{DiskError::WriteOutOfBounds, std::to_string(address)});

	while (numBytes--) {
		data[address + numBytes] = value & 0xFF;
		value >>= 8;
	}

	return {};
}

} // namespace Chaos