#include "chaos/vm/bus.hpp"

namespace Chaos {

Bus::Bus(Memory &ram_, Memory &video_) : ram(ram_), video(video_) {};

} // namespace Chaos