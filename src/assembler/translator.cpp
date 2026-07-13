#include "chaos/assembler/translator.hpp"
#include "chaos/common/enums.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace Chaos {

std::expected<void, Error> Assembler::assemble(const std::string &path) {
	auto addressMapResult = createAddressMap(path);

	if (!addressMapResult) {
		return std::unexpected(addressMapResult.error());
	}

	auto stringMapResult = createStringMap(path);

	if (!stringMapResult) {
		return std::unexpected(stringMapResult.error());
	}

	std::ifstream file(path);

	if (!file.is_open()) {
		return std::unexpected(Error{AssembleError::FileNotFound, path});
	}

	const std::string binaryPath = path + ".bin";
	std::ofstream binary(binaryPath, std::ios::binary);

	std::string line;

	while (std::getline(file, line)) {
		auto args = splitArguments(line);

		if (args.empty()) {
			continue;
		}

		if (args[0].starts_with("//")) {
			continue;
		}

		if (args[0] == "STRING") {
			if (args.size() != 3) {
				return std::unexpected(Error{AssembleError::InvalidArgumentCount, args[0]});
			}

			std::string_view str = args[2];

			for (const char &c : str) {
				binary.write(&c, 1);
			}

			continue;
		}

		auto instruction = createInstruction(args, addressMapResult.value(), stringMapResult.value());

		if (!instruction) {
			return std::unexpected(instruction.error());
		}

		for (int i = 7; i >= 0; --i) {
			char byte = static_cast<char>((instruction.value() >> (i * 8)) & 0xFF);
			binary.write(&byte, 1);
		}
	}

	return {};
}

std::expected<std::unordered_map<std::string, std::uint64_t>, Error> Assembler::createAddressMap(const std::string &path) {
	std::ifstream file(path);

	if (!file.is_open()) {
		return std::unexpected(Error{AssembleError::FileNotFound, path});
	}

	std::unordered_map<std::string, std::uint64_t> addressMap;
	std::uint64_t address = 0;
	std::string line;

	while (std::getline(file, line)) {
		auto args = splitArguments(line);

		if (args.empty()) {
			continue;
		}

		if (args[0].starts_with("//")) {
			continue;
		}

		if (args[0] == "LABEL") {
			if (args.size() != 2) {
				return std::unexpected(Error{AssembleError::InvalidArgumentCount, args[0]});
			}

			addressMap[args[1]] = address;
		}

		address += 8;
	}

	return addressMap;
}

std::expected<std::unordered_map<std::string, std::uint64_t>, Error> Assembler::createStringMap(const std::string &path) {
	std::ifstream file(path);

	if (!file.is_open()) {
		return std::unexpected(Error{AssembleError::FileNotFound, path});
	}

	std::unordered_map<std::string, std::uint64_t> stringMap;
	std::uint64_t address = 0;
	std::string line;

	while (std::getline(file, line)) {
		auto args = splitArguments(line);

		if (args.empty()) {
			continue;
		}

		if (args[0].starts_with("//")) {
			continue;
		}

		if (args[0] == "STRING") {
			if (args.size() != 3) {
				return std::unexpected(Error{AssembleError::InvalidArgumentCount, args[0]});
			}

			stringMap[args[1]] = address;

			address += args[2].length();
		} else {
			address += 8;
		}
	}

	return stringMap;
}

std::expected<std::uint64_t, Error> Assembler::createInstruction(const std::vector<std::string> &args,
                                                                 const std::unordered_map<std::string, std::uint64_t> &addressMap,
                                                                 const std::unordered_map<std::string, std::uint64_t> &stringMap) {
	auto opcode = enumFromString<Opcode>(args[0]);

	if (!opcode) {
		return std::unexpected(Error{AssembleError::UnknownOpcode, args[0]});
	}

	switch (opcode.value()) {
	case Opcode::HALT:
	case Opcode::STRING:
		return 0;

	case Opcode::LOAD:
	case Opcode::LOAD8:
	case Opcode::LOAD16:
	case Opcode::LOAD32:
	case Opcode::MOV:
	case Opcode::STORE:
	case Opcode::STORE8:
	case Opcode::STORE16:
	case Opcode::STORE32:
	case Opcode::CMP:
		return instructionRegisterRegister(opcode.value(), args);

	case Opcode::LOADI:
	case Opcode::LOADI8:
	case Opcode::LOADI16:
	case Opcode::LOADI32:
	case Opcode::MOVI:
	case Opcode::STOREI:
	case Opcode::STOREI8:
	case Opcode::STOREI16:
	case Opcode::STOREI32:
	case Opcode::CMPI:
		// case Opcode::SET:
		return instructionRegisterImmediate(opcode.value(), args);

	case Opcode::ADD:
	case Opcode::SUB:
		return instructionRegisterRegisterRegister(opcode.value(), args);

	case Opcode::ADDI:
	case Opcode::SUBI:
		return instructionRegisterRegisterImmediate(opcode.value(), args);

	case Opcode::LABEL:
	case Opcode::JMP:
	case Opcode::JZ:
	case Opcode::JNZ:
	case Opcode::JE:
	case Opcode::JNE:
	case Opcode::JG:
	case Opcode::JL:
	case Opcode::JGE:
	case Opcode::JLE:
		return instructionLabel(opcode.value(), args, addressMap);

	case Opcode::SYS:
		return instructionOnly(opcode.value(), args);

	case Opcode::CALL:
		return instructionLabel(opcode.value(), args, addressMap);

	case Opcode::RET:
		return instructionOnly(opcode.value(), args);

	case Opcode::MOVSTR:
		return instructionRegisterString(opcode.value(), args, stringMap);
	}

	return std::unexpected(Error{AssembleError::UnknownOpcode, args[0]});
}

std::expected<std::uint64_t, Error> Assembler::instructionOnly(Opcode opcode, const std::vector<std::string> &args) {
	if (args.size() != 1) {
		return std::unexpected(Error{AssembleError::InvalidArgumentCount, args[0]});
	}

	std::uint64_t result = 0;

	writeOpcode(result, opcode);

	return result;
}

std::expected<std::uint64_t, Error> Assembler::instructionLabel(Opcode opcode, const std::vector<std::string> &args,
                                                                const std::unordered_map<std::string, std::uint64_t> &addressMap) {
	if (args.size() != 2) {
		return std::unexpected(Error{AssembleError::InvalidArgumentCount, args[0]});
	}

	auto label = args[1];

	auto it = addressMap.find(label);
	if (it == addressMap.end()) {
		return std::unexpected(Error{AssembleError::UnknownLabel, label});
	}

	std::uint64_t address = it->second;

	std::uint64_t result = 0;

	writeOpcode(result, opcode);
	writeImmediate(result, address);

	return result;
}

std::expected<std::uint64_t, Error> Assembler::instructionRegisterString(Opcode opcode, const std::vector<std::string> &args,
                                                                         const std::unordered_map<std::string, std::uint64_t> &stringMap) {
	if (args.size() != 3) {
		return std::unexpected(Error{AssembleError::InvalidArgumentCount, args[0]});
	}

	auto registerDest = enumFromString<Register>(args[1]);

	if (!registerDest) {
		return std::unexpected(Error{AssembleError::UnknownRegister, args[1]});
	}

	auto str = args[2];

	auto it = stringMap.find(str);
	if (it == stringMap.end()) {
		return std::unexpected(Error{AssembleError::UnknownString, str});
	}

	std::uint64_t address = it->second;

	std::uint64_t result = 0;

	writeOpcode(result, opcode);
	writeRegisterDest(result, registerDest.value());
	writeImmediate(result, address);

	return result;
}

std::expected<std::uint64_t, Error> Assembler::instructionImmediate(Opcode opcode, const std::vector<std::string> &args) {
	if (args.size() != 2) {
		return std::unexpected(Error{AssembleError::InvalidArgumentCount, args[0]});
	}

	auto immediate = parseNumber(args[1]);

	if (!immediate) {
		return std::unexpected(immediate.error());
	}

	std::uint64_t result = 0;

	writeOpcode(result, opcode);
	writeImmediate(result, immediate.value());

	return result;
}

std::expected<std::uint64_t, Error> Assembler::instructionRegisterRegister(Opcode opcode, const std::vector<std::string> &args) {
	if (args.size() != 3) {
		return std::unexpected(Error{AssembleError::InvalidArgumentCount, args[0]});
	}

	auto registerDest = enumFromString<Register>(args[1]);

	if (!registerDest) {
		return std::unexpected(Error{AssembleError::UnknownRegister, args[1]});
	}

	auto registerSrc1 = enumFromString<Register>(args[2]);

	if (!registerSrc1) {
		return std::unexpected(Error{AssembleError::UnknownRegister, args[2]});
	}

	std::uint64_t result = 0;

	writeOpcode(result, opcode);
	writeRegisterDest(result, registerDest.value());
	writeRegisterSrc1(result, registerSrc1.value());

	return result;
}

std::expected<std::uint64_t, Error> Assembler::instructionRegisterImmediate(Opcode opcode, const std::vector<std::string> &args) {
	if (args.size() != 3) {
		return std::unexpected(Error{AssembleError::InvalidArgumentCount, args[0]});
	}

	auto registerDest = enumFromString<Register>(args[1]);

	if (!registerDest) {
		return std::unexpected(Error{AssembleError::UnknownRegister, args[1]});
	}

	auto immediate = parseNumber(args[2]);

	if (!immediate) {
		return std::unexpected(immediate.error());
	}

	std::uint64_t result = 0;

	writeOpcode(result, opcode);
	writeRegisterDest(result, registerDest.value());
	writeImmediate(result, immediate.value());

	return result;
}

std::expected<std::uint64_t, Error> Assembler::instructionRegisterRegisterRegister(Opcode opcode, const std::vector<std::string> &args) {
	if (args.size() != 4) {
		return std::unexpected(Error{AssembleError::InvalidArgumentCount, args[0]});
	}

	auto registerDest = enumFromString<Register>(args[1]);

	if (!registerDest) {
		return std::unexpected(Error{AssembleError::UnknownRegister, args[1]});
	}

	auto registerSrc1 = enumFromString<Register>(args[2]);

	if (!registerSrc1) {
		return std::unexpected(Error{AssembleError::UnknownRegister, args[2]});
	}

	auto registerSrc2 = enumFromString<Register>(args[3]);

	if (!registerSrc2) {
		return std::unexpected(Error{AssembleError::UnknownRegister, args[3]});
	}

	std::uint64_t result = 0;

	writeOpcode(result, opcode);
	writeRegisterDest(result, registerDest.value());
	writeRegisterSrc1(result, registerSrc1.value());
	writeRegisterSrc2(result, registerSrc2.value());

	return result;
}

std::expected<std::uint64_t, Error> Assembler::instructionRegisterRegisterImmediate(Opcode opcode, const std::vector<std::string> &args) {
	if (args.size() != 4) {
		return std::unexpected(Error{AssembleError::InvalidArgumentCount, args[0]});
	}

	auto registerDest = enumFromString<Register>(args[1]);

	if (!registerDest) {
		return std::unexpected(Error{AssembleError::UnknownRegister, args[1]});
	}

	auto registerSrc1 = enumFromString<Register>(args[2]);

	if (!registerSrc1) {
		return std::unexpected(Error{AssembleError::UnknownRegister, args[2]});
	}

	auto immediate = parseNumber(args[3]);

	if (!immediate) {
		return std::unexpected(immediate.error());
	}

	std::uint64_t result = 0;

	writeOpcode(result, opcode);
	writeRegisterDest(result, registerDest.value());
	writeRegisterSrc1(result, registerSrc1.value());
	writeImmediate(result, immediate.value());

	return result;
}

void Assembler::writeOpcode(std::uint64_t &instruction, Opcode opcode) {
	const auto opcodeValue = static_cast<std::uint64_t>(opcode);
	instruction |= opcodeValue << 56;
}

void Assembler::writeRegisterDest(std::uint64_t &instruction, Register reg) {
	const auto registerValue = static_cast<std::uint64_t>(reg);
	instruction |= registerValue << 48;
}

void Assembler::writeRegisterSrc1(std::uint64_t &instruction, Register reg) {
	const auto registerValue = static_cast<std::uint64_t>(reg);
	instruction |= registerValue << 40;
}

void Assembler::writeRegisterSrc2(std::uint64_t &instruction, Register reg) {
	const auto registerValue = static_cast<std::uint64_t>(reg);
	instruction |= registerValue << 32;
}

void Assembler::writeImmediate(std::uint64_t &instruction, std::uint64_t imm) {
	instruction |= (imm & 0xFF);
	imm >>= 8;

	instruction |= (imm & 0xFF) << 8;
	imm >>= 8;

	instruction |= (imm & 0xFF) << 16;
	imm >>= 8;

	instruction |= (imm & 0xFF) << 24;
}

std::vector<std::string> Assembler::splitArguments(const std::string &line) {
	std::istringstream iss(line);
	std::string arg;
	std::vector<std::string> args;

	while (iss >> arg) {
		args.push_back(arg);
	}

	return args;
}

std::expected<std::uint64_t, Error> Assembler::parseNumber(const std::string &imm) {
	try {
		std::size_t pos;
		const auto value = std::stoull(imm, &pos, 0);

		if (pos != imm.size()) {
			return std::unexpected(Error{AssembleError::InvalidIntegerFormat, imm});
		}

		return static_cast<std::uint64_t>(value);
	} catch (const std::invalid_argument &) {
		return std::unexpected(Error{AssembleError::InvalidIntegerFormat, imm});
	} catch (const std::out_of_range &) {
		return std::unexpected(Error{AssembleError::InvalidIntegerFormat, imm});
	}
}

} // namespace Chaos