#include "chaos/vm/chip.hpp"
#include "chaos/common/enums.hpp"

namespace Chaos {

Chip::Chip(Cpu &cpu_, Bus &bus_) : cpu(cpu_), bus(bus_) {};

std::expected<void, Error> Chip::run() {
	auto result = cpu.execute(bus);

	if (!result) {
		return std::unexpected(result.error());
	}

	auto pc = cpu.readRegister(Register::PC);
	auto po = cpu.readRegister(Register::PO);

	cpu.writeRegister(Register::PC, pc + 8);

	return {};
}

std::expected<void, Error> Chip::printCpuState() const {
	for (size_t i = 0; i < RegisterCount; i++) {
		auto reg = static_cast<Register>(i);
		auto registerValue = cpu.readRegister(reg);

		std::cout << stringFromEnum(reg) << ": " << registerValue << "\n";
	}

	return {};
}

} // namespace Chaos