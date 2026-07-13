#pragma once

#include <expected>

#include "chaos/common/errors.hpp"
#include "chaos/vm/disk.hpp"

namespace Chaos {

enum class ImgError { FileNotFound, ReadError, WriteError };

class Img {
  public:
	static std::expected<void, Error> save(Disk &disk, const std::string &path);
	static std::expected<void, Error> load(Disk &disk, const std::string &path);
};

} // namespace Chaos