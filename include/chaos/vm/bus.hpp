#pragma once

#include <cstdint>
#include <expected>

#include "chaos/common/errors.hpp"
#include "chaos/vm/memory.hpp"
#include "chaos/vm/disk.hpp"

namespace Chaos {

class Bus {
  public:
	template <typename T> std::expected<T, Error> read(std::uint64_t address);
	template <typename T> std::expected<void, Error> write(std::uint64_t address, T value);

  private:
	Memory memory;
};

template <typename T> std::expected<T, Error> Bus::read(std::uint64_t address) { return memory.read<T>(address); }
template <typename T> std::expected<void, Error> Bus::write(std::uint64_t address, T value) { return memory.write<T>(address, value); }

} // namespace Chaos