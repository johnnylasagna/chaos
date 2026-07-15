#include "chaos/vm/cpu.hpp"

namespace Chaos {

Cpu::Cpu() {
	registers.resize(RegisterCount);
	flags.resize(FlagCount);
	trap = false;

	writeRegister(Register::RETSP, 4096);
}

std::expected<void, Error> Cpu::execute(Bus &bus) {
	auto instruction = fetchInstruction(bus);

	if (!instruction) {
		return std::unexpected(instruction.error());
	}

	auto result = executeInstruction(*instruction, bus);

	if (!result) {
		return std::unexpected(result.error());
	}

	return {};
}

std::expected<Instruction, Error> Cpu::fetchInstruction(Bus &bus) {
	auto pc = readRegister(Register::PC);
	auto po = readRegister(Register::PO);

	auto result = bus.read<std::uint64_t>(po + pc);

	if (!result) {
		return std::unexpected(result.error());
	}

	const std::uint64_t data = result.value();

	auto opcode = readOpcode(data);

	if (!opcode) {
		return std::unexpected(opcode.error());
	}

	auto registerDest = readRegisterDest(data);

	if (!registerDest) {
		return std::unexpected(registerDest.error());
	}

	auto registerSrc1 = readRegisterSrc1(data);

	if (!registerSrc1) {
		return std::unexpected(registerSrc1.error());
	}

	auto registerSrc2 = readRegisterSrc2(data);

	if (!registerSrc2) {
		return std::unexpected(registerSrc2.error());
	}

	return Instruction{opcode.value(), registerDest.value(), registerSrc1.value(), registerSrc2.value(), readImmediate(data)};
}

std::expected<void, Error> Cpu::executeInstruction(const Instruction &instruction, Bus &bus) {
	switch (instruction.opcode) {

	case Opcode::HALT:
		break;

	case Opcode::LOAD: {
		auto result = executeLoad<uint64_t>(instruction, bus);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::LOAD8: {
		auto result = executeLoad<uint8_t>(instruction, bus);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::LOAD16: {
		auto result = executeLoad<uint16_t>(instruction, bus);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::LOAD32: {
		auto result = executeLoad<uint32_t>(instruction, bus);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::LOADI: {
		auto result = executeLoadI<uint64_t>(instruction, bus);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::LOADI8: {
		auto result = executeLoadI<uint8_t>(instruction, bus);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::LOADI16: {
		auto result = executeLoadI<uint16_t>(instruction, bus);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::LOADI32: {
		auto result = executeLoadI<uint32_t>(instruction, bus);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::STORE: {
		auto result = executeStore<uint64_t>(instruction, bus);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::STORE8: {
		auto result = executeStore<uint8_t>(instruction, bus);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::STORE16: {
		auto result = executeStore<uint16_t>(instruction, bus);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::STORE32: {
		auto result = executeStore<uint32_t>(instruction, bus);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::STOREI: {
		auto result = executeStoreI<uint64_t>(instruction, bus);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::STOREI8: {
		auto result = executeStoreI<uint8_t>(instruction, bus);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::STOREI16: {
		auto result = executeStoreI<uint16_t>(instruction, bus);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::STOREI32: {
		auto result = executeStoreI<uint32_t>(instruction, bus);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::MOV: {
		auto value = readRegister(instruction.registerSrc1);

		writeRegister(instruction.registerDest, value);

		break;
	}

	case Opcode::MOVI: {
		uint64_t value = instruction.immediate;

		writeRegister(instruction.registerDest, value);

		break;
	}

	case Opcode::ADD: {
		auto value1 = readRegister(instruction.registerSrc1);
		auto value2 = readRegister(instruction.registerSrc2);

		uint64_t resultValue = value1 + value2;
		writeRegister(instruction.registerDest, resultValue);

		break;
	}

	case Opcode::ADDI: {
		auto value1 = readRegister(instruction.registerSrc1);

		uint64_t resultValue = value1 + instruction.immediate;
		writeRegister(instruction.registerDest, resultValue);

		break;
	}

	case Opcode::SUB: {
		auto value1 = readRegister(instruction.registerSrc1);
		auto value2 = readRegister(instruction.registerSrc2);

		uint64_t resultValue = value1 - value2;
		writeRegister(instruction.registerDest, resultValue);

		break;
	}

	case Opcode::SUBI: {
		auto value1 = readRegister(instruction.registerSrc1);

		uint64_t resultValue = value1 - instruction.immediate;
		writeRegister(instruction.registerDest, resultValue);

		break;
	}

	case Opcode::LABEL:
		break;

	case Opcode::JMP: {
		writeRegister(Register::PC, instruction.immediate);
		break;
	}

	case Opcode::CMP: {
		auto value1 = readRegister(instruction.registerDest);
		auto value2 = readRegister(instruction.registerSrc2);

		if (value1 == value2) {
			writeFlag(Flag::Zero, 1);
		} else {
			writeFlag(Flag::Zero, 0);
		}

		if (value1 > value2) {
			writeFlag(Flag::Greater, 1);
		} else {
			writeFlag(Flag::Greater, 0);
		}

		if (value1 < value2) {
			writeFlag(Flag::Less, 1);
		} else {
			writeFlag(Flag::Less, 0);
		}

		break;
	}

	case Opcode::CMPI: {
		auto value1 = readRegister(instruction.registerDest);

		if (value1 == instruction.immediate) {
			writeFlag(Flag::Zero, 1);
		} else {
			writeFlag(Flag::Zero, 0);
		}

		if (value1 > instruction.immediate) {
			writeFlag(Flag::Greater, 1);
		} else {
			writeFlag(Flag::Greater, 0);
		}

		if (value1 < instruction.immediate) {
			writeFlag(Flag::Less, 1);
		} else {
			writeFlag(Flag::Less, 0);
		}

		break;
	}

	case Opcode::JZ: {
		auto zeroFlag = readFlag(Flag::Zero);

		if (zeroFlag == 1) {
			writeRegister(Register::PC, instruction.immediate);
		}

		break;
	}

	case Opcode::JNZ: {
		auto zeroFlag = readFlag(Flag::Zero);

		if (zeroFlag == 0) {
			writeRegister(Register::PC, instruction.immediate);
		}

		break;
	}

	case Opcode::JE: {
		auto zeroFlag = readFlag(Flag::Zero);

		if (zeroFlag == 1) {
			writeRegister(Register::PC, instruction.immediate);
		}

		break;
	}

	case Opcode::JNE: {
		auto zeroFlag = readFlag(Flag::Zero);

		if (zeroFlag == 0) {
			writeRegister(Register::PC, instruction.immediate);
		}

		break;
	}

	case Opcode::JG: {
		auto greaterFlag = readFlag(Flag::Greater);

		if (greaterFlag == 1) {
			writeRegister(Register::PC, instruction.immediate);
		}

		break;
	}

	case Opcode::JL: {
		auto lessFlag = readFlag(Flag::Less);

		if (lessFlag == 1) {
			writeRegister(Register::PC, instruction.immediate);
		}

		break;
	}

	case Opcode::JGE: {
		auto lessFlag = readFlag(Flag::Less);

		if (lessFlag == 0) {
			writeRegister(Register::PC, instruction.immediate);
		}

		break;
	}

	case Opcode::JLE: {
		auto greaterFlag = readFlag(Flag::Greater);

		if (greaterFlag == 0) {
			writeRegister(Register::PC, instruction.immediate);
		}

		break;
	}

	case Opcode::SYS: {
		trap = true;
		break;
	}

	case Opcode::CALL: {
		auto programCounter = readRegister(Register::PC);
		auto programOffset = readRegister(Register::PO);
		auto rsp = readRegister(Register::RETSP);

		rsp -= 8;
		auto pcWriteResult = bus.write(rsp, programCounter);
		if (!pcWriteResult)
			return std::unexpected(pcWriteResult.error());

		rsp -= 8;
		auto poWriteResult = bus.write(rsp, programOffset);
		if (!poWriteResult)
			return std::unexpected(poWriteResult.error());

		writeRegister(Register::RETSP, rsp);

		auto address = instruction.immediate;
		writeRegister(Register::PC, address);
		break;
	}

	case Opcode::RET: {
		auto rsp = readRegister(Register::RETSP);

		auto poReadResult = bus.read<uint64_t>(rsp);
		if (!poReadResult)
			return std::unexpected(poReadResult.error());
		rsp += 8;

		auto pcReadResult = bus.read<uint64_t>(rsp);
		if (!pcReadResult)
			return std::unexpected(pcReadResult.error());
		rsp += 8;

		writeRegister(Register::RETSP, rsp);
		writeRegister(Register::PC, pcReadResult.value());
		writeRegister(Register::PO, poReadResult.value());
		break;
	}

	case Opcode::MOVSTR: {
		auto value = instruction.immediate;
		writeRegister(instruction.registerDest, value);

		break;
	}

	case Opcode::STRING: {
		break;
	}
	}

	return {};
}

std::expected<Opcode, Error> Cpu::readOpcode(std::uint64_t instruction) {
	const auto opcodeValue = instruction >> 56;

	if (opcodeValue > OpcodeCount) {
		return std::unexpected(Error{CpuError::InvalidOpcode, std::to_string(opcodeValue)});
	}

	Opcode opcode = static_cast<Opcode>(opcodeValue);

	return opcode;
}

std::expected<Register, Error> Cpu::readRegisterDest(std::uint64_t instruction) {
	const auto registerValue = (instruction >> 48) & 0xFF;

	if (registerValue >= RegisterCount) {
		return std::unexpected(Error{CpuError::InvalidRegister, std::to_string(registerValue)});
	}
	Register reg = static_cast<Register>(registerValue);
	return reg;
}

std::expected<Register, Error> Cpu::readRegisterSrc1(std::uint64_t instruction) {
	const auto registerValue = (instruction >> 40) & 0xFF;

	if (registerValue >= RegisterCount) {
		return std::unexpected(Error{CpuError::InvalidRegister, std::to_string(registerValue)});
	}

	Register reg = static_cast<Register>(registerValue);
	return reg;
}

std::expected<Register, Error> Cpu::readRegisterSrc2(std::uint64_t instruction) {
	const auto registerValue = (instruction >> 32) & 0xFF;

	if (registerValue >= RegisterCount) {
		return std::unexpected(Error{CpuError::InvalidRegister, std::to_string(registerValue)});
	}

	Register reg = static_cast<Register>(registerValue);
	return reg;
}

uint64_t Cpu::readImmediate(std::uint64_t instruction) {
	const auto value = instruction & 0xFFFFFFFF;
	return value;
}

uint64_t Cpu::readRegister(Register reg) const {
	size_t registerValue = static_cast<size_t>(reg);
	return registers[registerValue];
}

void Cpu::writeRegister(Register reg, uint64_t value) {
	size_t registerValue = static_cast<size_t>(reg);
	registers[registerValue] = value;
}

uint64_t Cpu::readFlag(Flag flag) const {
	size_t flagValue = static_cast<size_t>(flag);
	return flags[flagValue];
}

void Cpu::writeFlag(Flag flag, uint8_t value) {
	size_t flagValue = static_cast<size_t>(flag);
	flags[flagValue] = value;
}

bool Cpu::getTrapStatus() const { return trap; }

void Cpu::setTrapStatus(bool status) { trap = status; }

} // namespace Chaos