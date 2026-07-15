#pragma once

#include "chaos/vm/memory.hpp"

#include <raylib.h>
#include <atomic>

namespace Chaos {

class Display {
  public:
	Display(Memory &buffer_);

	void show();
	~Display();

  private:
	Memory &video;
	std::vector<uint8_t> buffer;
	Texture2D texture;
	Image image;
};

} // namespace Chaos