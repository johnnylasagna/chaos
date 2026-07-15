#include "chaos/vm/memory.hpp"

namespace Chaos {

Memory::Memory(const size_t size) { data.resize(size); }

std::expected<void, Error> Memory::copy(std::vector<uint8_t> &dest) {
	std::shared_lock<std::shared_mutex> lock(mtx);
    std::copy(data.begin(), data.end(), dest.begin());

    return {};
}

} // namespace Chaos