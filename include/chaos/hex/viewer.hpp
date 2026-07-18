#pragma once

#include <string>
#include <expected>
#include <ftxui/dom/elements.hpp>

#include "chaos/common/errors.hpp"

namespace Chaos {

enum class ViewerError {
    FileOpenFailed,
    FileReadFailed,
    ViewOutOfRange
};

class Viewer {
  public:
	std::expected<void, Error> open(const std::string &path);
	ftxui::Element render(size_t address, size_t size) const;

  private:
	std::string bytes;
};

} // namespace Chaos