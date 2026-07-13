#pragma once

#include <print>

#include "chaos/common/enums.hpp"

namespace Chaos {

struct Error {
	std::string code;
	std::string context;

	template <typename T>
	    requires std::is_enum_v<T>
	Error(T error, std::string context) : code(stringFromEnum(error)), context(std::move(context)) {}
};

inline void printError(const Error &error) { std::println("Error: {} at: {}", error.code, error.context); }

} // namespace Chaos
