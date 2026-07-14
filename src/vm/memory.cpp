#include "chaos/vm/memory.hpp"

namespace Chaos {

Memory::Memory(const size_t size) { data.resize(size); }

} // namespace Chaos