#include "chaos/vm/cpu.hpp"

namespace Chaos {

Cpu::Cpu() {
	registers.resize(RegisterCount);
	flags.resize(FlagCount);

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
		uint64_t value1 = readRegister(instruction.registerSrc1);
		uint64_t value2 = readRegister(instruction.registerSrc2);

		__uint128_t result = (__uint128_t)value1 + value2;

		writeRegister(instruction.registerDest, (uint64_t)result);
		writeRegister(Register::Overflow, (uint64_t)(result >> 64));

		break;
	}

	case Opcode::ADDI: {
		uint64_t value1 = readRegister(instruction.registerSrc1);

		__uint128_t result = (__uint128_t)value1 + instruction.immediate;

		writeRegister(instruction.registerDest, (uint64_t)result);
		writeRegister(Register::Overflow, (uint64_t)(result >> 64));

		break;
	}

	case Opcode::SUB: {
		uint64_t value1 = readRegister(instruction.registerSrc1);
		uint64_t value2 = readRegister(instruction.registerSrc2);

		uint64_t result = value1 - value2;

		writeRegister(instruction.registerDest, result);
		writeRegister(Register::Overflow, value1 < value2);

		break;
	}

	case Opcode::SUBI: {
		uint64_t value1 = readRegister(instruction.registerSrc1);

		uint64_t result = value1 - instruction.immediate;

		writeRegister(instruction.registerDest, result);
		writeRegister(Register::Overflow, value1 < instruction.immediate);

		break;
	}

	case Opcode::MUL: {
		uint64_t value1 = readRegister(instruction.registerSrc1);
		uint64_t value2 = readRegister(instruction.registerSrc2);

		__uint128_t result = (__uint128_t)value1 * value2;

		writeRegister(instruction.registerDest, (uint64_t)result);
		writeRegister(Register::Overflow, (uint64_t)(result >> 64));

		break;
	}

	case Opcode::MULI: {
		uint64_t value1 = readRegister(instruction.registerSrc1);

		__uint128_t result = (__uint128_t)value1 * instruction.immediate;

		writeRegister(instruction.registerDest, (uint64_t)result);
		writeRegister(Register::Overflow, (uint64_t)(result >> 64));

		break;
	}

	case Opcode::DIV: {
		uint64_t value1 = readRegister(instruction.registerSrc1);
		uint64_t value2 = readRegister(instruction.registerSrc2);

		if (value2 == 0) {
			return std::unexpected(Error{CpuError::DivisionByZero, "DIV"});
		}

		writeRegister(instruction.registerDest, value1 / value2);
		writeRegister(Register::Overflow, value1 % value2);

		break;
	}

	case Opcode::DIVI: {
		uint64_t value1 = readRegister(instruction.registerSrc1);

		if (instruction.immediate == 0) {
			return std::unexpected(Error{CpuError::DivisionByZero, "DIVI"});
		}

		writeRegister(instruction.registerDest, value1 / instruction.immediate);
		writeRegister(Register::Overflow, value1 % instruction.immediate);

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

		auto programCounter = readRegister(Register::PC);
		auto programOffset = readRegister(Register::PO);
		auto rsp = readRegister(Register::RETSP);

		rsp -= 8;
		auto pcWriteResult = bus.write(rsp, programCounter);

		if (!pcWriteResult) {
			return std::unexpected(pcWriteResult.error());
		}

		rsp -= 8;
		auto poWriteResult = bus.write(rsp, programOffset);

		if (!poWriteResult) {
			return std::unexpected(poWriteResult.error());
		}

		writeRegister(Register::RETSP, rsp);

		writeRegister(Register::PC, 0);
		writeRegister(Register::PO, 0x200);

		break;
	}

	case Opcode::SYS_READ: {
		if (!trap) {
			return std::unexpected(Error{CpuError::NotPrivileged, "SYS_READ"});
		}

		auto diskAddress = readRegister(Register::B);
		auto memoryAddress = readRegister(Register::C);
		auto size = readRegister(Register::D);

		auto result = bus.copyFromDisk(diskAddress, memoryAddress, size);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::SYS_WRITE: {
		if (!trap) {
			return std::unexpected(Error{CpuError::NotPrivileged, "SYS_WRITE"});
		}

		auto diskAddress = readRegister(Register::B);
		auto memoryAddress = readRegister(Register::C);
		auto size = readRegister(Register::D);

		auto result = bus.copyToDisk(diskAddress, memoryAddress, size);

		if (!result) {
			return std::unexpected(result.error());
		}

		break;
	}

	case Opcode::SYS_PRINT: {
		if (!trap) {
			return std::unexpected(Error{CpuError::NotPrivileged, "SYS_PRINT"});
		}

		constexpr int Scale = 2;

		auto character = readRegister(Register::B);
		auto x = readRegister(Register::C);
		auto y = readRegister(Register::D);

		auto base = CharsOffset + character * 8;

		for (int row = 0; row < 8; ++row) {
			uint8_t bits = bus.read<uint8_t>(base + row).value();

			for (int col = 0; col < 8; ++col) {
				bool on = bits & (0x80 >> col);

				for (int dy = 0; dy < Scale; ++dy) {
					for (int dx = 0; dx < Scale; ++dx) {
						auto result =
						    bus.write<uint8_t>(VideoOffset + (y + row * Scale + dy) * VideoWidth + (x + col * Scale + dx), on ? 255 : 0);

						if (!result) {
							return std::unexpected(result.error());
						}
					}
				}
			}
		}

		break;
	}

	case Opcode::CALL: {
		auto programCounter = readRegister(Register::PC);
		auto programOffset = readRegister(Register::PO);
		auto rsp = readRegister(Register::RETSP);

		rsp -= 8;
		auto pcWriteResult = bus.write(rsp, programCounter);

		if (!pcWriteResult) {
			return std::unexpected(pcWriteResult.error());
		}

		rsp -= 8;
		auto poWriteResult = bus.write(rsp, programOffset);

		if (!poWriteResult) {
			return std::unexpected(poWriteResult.error());
		}

		writeRegister(Register::RETSP, rsp);

		auto address = instruction.immediate;
		writeRegister(Register::PC, address);

		break;
	}

	case Opcode::RET: {
		auto rsp = readRegister(Register::RETSP);

		auto poReadResult = bus.read<uint64_t>(rsp);

		if (!poReadResult) {
			return std::unexpected(poReadResult.error());
		}

		rsp += 8;

		auto pcReadResult = bus.read<uint64_t>(rsp);

		if (!pcReadResult) {
			return std::unexpected(pcReadResult.error());
		}

		rsp += 8;

		writeRegister(Register::RETSP, rsp);
		writeRegister(Register::PC, pcReadResult.value());
		writeRegister(Register::PO, poReadResult.value());

		break;
	}

	case Opcode::KRET: {
		trap = false;

		auto rsp = readRegister(Register::RETSP);

		auto poReadResult = bus.read<uint64_t>(rsp);

		if (!poReadResult) {
			return std::unexpected(poReadResult.error());
		}

		rsp += 8;

		auto pcReadResult = bus.read<uint64_t>(rsp);

		if (!pcReadResult) {
			return std::unexpected(pcReadResult.error());
		}

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

std::pair<std::vector<std::uint64_t>, std::vector<std::uint8_t>> Cpu::getState() const {
	return {registers, flags};
}

} // namespace Chaos