#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <vector>
#include <unordered_map>

#include "chaos/common/instructions.hpp"
#include "chaos/common/errors.hpp"

namespace Chaos {

enum class AssembleError {
	FileNotFound,
	UnknownOpcode,
	UnknownRegister,
	InvalidArgumentCount,
	InvalidIntegerFormat,
	UnknownLabel,
	UnknownString
};

class Assembler {
  public:
	static std::expected<void, Error> assemble(const std::string &path);

  private:
	static std::expected<std::unordered_map<std::string, std::uint64_t>, Error> createAddressMap(const std::string &path);
	static std::expected<std::unordered_map<std::string, std::uint64_t>, Error> createStringMap(const std::string &path);

	static std::expected<std::uint64_t, Error> createInstruction(const std::vector<std::string> &args,
	                                                             const std::unordered_map<std::string, std::uint64_t> &addressMap,
	                                                             const std::unordered_map<std::string, std::uint64_t> &stringMap);

	static std::expected<std::uint64_t, Error> instructionLabel(Opcode opcode, const std::vector<std::string> &args,
	                                                            const std::unordered_map<std::string, std::uint64_t> &addressMap);
	static std::expected<std::uint64_t, Error> instructionRegisterString(Opcode opcode, const std::vector<std::string> &args,
	                                                                     const std::unordered_map<std::string, std::uint64_t> &stringMap);
	static std::expected<std::uint64_t, Error> instructionOnly(Opcode opcode, const std::vector<std::string> &args);
	static std::expected<std::uint64_t, Error> instructionImmediate(Opcode opcode, const std::vector<std::string> &args);
	static std::expected<std::uint64_t, Error> instructionRegisterRegister(Opcode opcode, const std::vector<std::string> &args);
	static std::expected<std::uint64_t, Error> instructionRegisterImmediate(Opcode opcode, const std::vector<std::string> &args);
	static std::expected<std::uint64_t, Error> instructionRegisterRegisterRegister(Opcode opcode, const std::vector<std::string> &args);
	static std::expected<std::uint64_t, Error> instructionRegisterRegisterImmediate(Opcode opcode, const std::vector<std::string> &args);

	static void writeOpcode(std::uint64_t &instruction, Opcode opcode);
	static void writeRegisterDest(std::uint64_t &instruction, Register reg);
	static void writeRegisterSrc1(std::uint64_t &instruction, Register reg);
	static void writeRegisterSrc2(std::uint64_t &instruction, Register reg);
	static void writeImmediate(std::uint64_t &instruction, std::uint64_t immediate);

	static std::vector<std::string> splitArguments(const std::string &line);
	static std::expected<std::uint64_t, Error> parseNumber(const std::string &imm);

	static void handleEscapeSequences(std::string &str);
};

} // namespace Chaos