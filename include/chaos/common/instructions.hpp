#pragma once

#include <cstdint>
#include <meta>
#include <unordered_map>
#include <string>
#include <optional>

namespace Chaos {

// After making any change to the opcode
// Make sure to update the following files
// 1. src/vm/cpu.cpp
// 2. src/assembler/translator.cpp

enum class Opcode {
	HALT,
	SYS,
	CALL,
	RET,
	LOAD,
	LOAD8,
	LOAD16,
	LOAD32,
	LOADI,
	LOADI8,
	LOADI16,
	LOADI32,
	STORE,
	STORE8,
	STORE16,
	STORE32,
	STOREI,
	STOREI8,
	STOREI16,
	STOREI32,
	MOV,
	MOVI,
	ADD,
	ADDI,
	SUB,
	SUBI,
	LABEL,
	JMP,
	CMP,
	CMPI,
	JZ,
	JNZ,
	JE,
	JNE,
	JG,
	JL,
	JGE,
	JLE,
	STRING,
	MOVSTR
	// SET,
};

// This line might give an error due to intellisense not supporting C++26 yet. You can ignore it.

inline constexpr size_t OpcodeCount = std::meta::enumerators_of(^^Opcode).size();

// After making any change to the register
// Make sure to update the following files
// 1. src/vm/cpu.cpp
// 2. src/assembler/translator.cpp

enum class Register {
	PC,
	PO,
	RETSP,
	A,
	B,
	C,
	D,
};

// This line might give an error due to intellisense not supporting C++26 yet. You can ignore it.

inline constexpr size_t RegisterCount = std::meta::enumerators_of(^^Register).size();

// After making any change to the flag
// Make sure to update the following files
// 1. src/vm/cpu.cpp
// 2. src/assembler/translator.cpp

enum class Flag {
	Sys,
	Zero,
	Greater,
	Less,
};

// This line might give an error due to intellisense not supporting C++26 yet. You can ignore it.

inline constexpr size_t FlagCount = std::meta::enumerators_of(^^Flag).size();

// Stores instructions in a struct for easy access and manipulation
// Opcode - 8 bits
// RegisterDest - 8 bits
// RegisterSrc1 - 8 bits
// RegisterSrc2 - 8 bits
// Immediate - 32 bits
struct Instruction {
	Opcode opcode;
	Register registerDest;
	Register registerSrc1;
	Register registerSrc2;
	uint64_t immediate;
};

enum class Syscall {
	Kernel,
	Copy,
	Print
};

} // namespace Chaos
