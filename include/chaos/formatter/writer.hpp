#pragma once

#include <expected>
#include <string>
#include <cstdint>

#include "chaos/common/errors.hpp"

namespace Chaos {

enum class WriterError { FileNotFound, ReadError, WriteError };

class Writer {
  public:

  static std::expected<void, Error> clear(const std::string &path, uint64_t address, size_t size);
	static std::expected<void, Error> write(const std::string &path, uint64_t address, const std::string &binaryPath);
};

} // namespace Chaos