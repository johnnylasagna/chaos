#pragma once

#include <cstdint>
#include <expected>
#include <vector>

#include "chaos/common/errors.hpp"
#include "chaos/common/instructions.hpp"
#include "chaos/vm/bus.hpp"

namespace Chaos {

enum class CpuError {
	InvalidOpcode,
	InvalidRegister,
	InvalidFlag,
};

class Cpu {
  public:
	Cpu();

	std::expected<void, Error> execute(Bus &bus);

	uint64_t readRegister(Register reg) const;
	void writeRegister(Register reg, std::uint64_t value);

	uint64_t readFlag(Flag flag) const;
	void writeFlag(Flag flag, std::uint8_t value);

	bool getTrapStatus() const;
	void setTrapStatus(bool status);

  private:
	std::vector<std::uint64_t> registers;
	std::vector<std::uint8_t> flags;
	bool trap;

	std::expected<Instruction, Error> fetchInstruction(Bus &bus);
	std::expected<void, Error> executeInstruction(const Instruction &instruction, Bus &bus);

	template <typename T> std::expected<void, Error> executeLoad(const Instruction &instruction, Bus &bus);
	template <typename T> std::expected<void, Error> executeLoadI(const Instruction &instruction, Bus &bus);
	template <typename T> std::expected<void, Error> executeStore(const Instruction &instruction, Bus &bus);
	template <typename T> std::expected<void, Error> executeStoreI(const Instruction &instruction, Bus &bus);

	static std::expected<Opcode, Error> readOpcode(std::uint64_t instruction);
	static std::expected<Register, Error> readRegisterDest(std::uint64_t instruction);
	static std::expected<Register, Error> readRegisterSrc1(std::uint64_t instruction);
	static std::expected<Register, Error> readRegisterSrc2(std::uint64_t instruction);
	static std::uint64_t readImmediate(std::uint64_t instruction);
};

template <typename T> std::expected<void, Error> Cpu::executeLoad(const Instruction &instruction, Bus &bus) {
	auto address = readRegister(instruction.registerSrc1);

	auto result = bus.read<T>(address);

	if (!result) {
		return std::unexpected(result.error());
	}

	writeRegister(instruction.registerDest, result.value());

	return {};
}

template <typename T> std::expected<void, Error> Cpu::executeLoadI(const Instruction &instruction, Bus &bus) {
	uint64_t immediate = instruction.immediate;
	auto result = bus.read<T>(immediate);

	if (!result) {
		return std::unexpected(result.error());
	}

	writeRegister(instruction.registerDest, result.value());

	return {};
}

template <typename T> std::expected<void, Error> Cpu::executeStore(const Instruction &instruction, Bus &bus) {
	auto value = readRegister(instruction.registerSrc1);

	auto address = readRegister(instruction.registerDest);

	auto result = bus.write<T>(address, value);

	if (!result) {
		return std::unexpected(result.error());
	}

	return {};
}

template <typename T> std::expected<void, Error> Cpu::executeStoreI(const Instruction &instruction, Bus &bus) {
	auto dest = readRegister(instruction.registerDest);
	auto value = instruction.immediate;

	auto result = bus.write<T>(dest, value);

	if (!result) {
		return std::unexpected(result.error());
	}

	return {};
}

} // namespace Chaos