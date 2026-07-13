#include "chaos/vm/chip.hpp"
#include "chaos/common/enums.hpp"

namespace Chaos {

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

std::expected<void, Error> Chip::copyFromDisk(Disk &disk, std::uint64_t diskAddress, std::uint64_t memoryAddress, std::size_t size) {
	for (std::size_t i = 0; i < size; ++i) {
		auto readResult = disk.read<std::uint8_t>(diskAddress + i);

		if (!readResult) {
			return std::unexpected(readResult.error());
		}

		auto writeResult = bus.write<std::uint8_t>(memoryAddress + i, readResult.value());

		if (!writeResult) {
			return std::unexpected(writeResult.error());
		}
	}

	return {};
}

std::expected<void, Error> Chip::copyToDisk(Disk &disk, std::uint64_t memoryAddress, std::uint64_t diskAddress, std::size_t size) {
	for (std::size_t i = 0; i < size; ++i) {
		auto readResult = bus.read<std::uint8_t>(memoryAddress + i);

		if (!readResult) {
			return std::unexpected(readResult.error());
		}

		auto writeResult = disk.write<std::uint8_t>(diskAddress + i, readResult.value());

		if (!writeResult) {
			return std::unexpected(writeResult.error());
		}
	}

	return {};
}

std::expected<void, Error> Chip::checkTrap(Disk &disk) {
	if (cpu.getTrapStatus()) {
		cpu.setTrapStatus(false);
		auto syscallResult = handleSystemCall(disk);

		if (!syscallResult) {
			return std::unexpected(syscallResult.error());
		}
	}

	return {};
}

std::expected<void, Error> Chip::handleSystemCall(Disk &disk) {
	auto call = cpu.readRegister(Register::A);

	auto syscall = static_cast<Syscall>(call);

	switch (syscall) {
	case Syscall::Kernel: {
		cpu.writeRegister(Register::PC, 0);
		cpu.writeRegister(Register::PO, 0x200);

		break;
	}

	case Syscall::Copy: {
		auto diskAddress = cpu.readRegister(Register::B);
		auto memoryAddress = cpu.readRegister(Register::C);
		auto size = cpu.readRegister(Register::D);

		auto result = copyFromDisk(disk, diskAddress, memoryAddress, size);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Syscall::Print: {
		auto character = cpu.readRegister(Register::B);
		std::cout << static_cast<char>(character);

		break;
	}

	default:
		return std::unexpected(Error{ChipError::UnknownSysCall, std::to_string(call)});
	}

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