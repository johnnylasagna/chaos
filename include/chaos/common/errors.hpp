#pragma once

#include <print>
#include <sstream>
#include <source_location>
#include <string>

#include "chaos/common/enums.hpp"

namespace Chaos {

// struct Error {
// 	std::string code;
// 	std::string context;

// 	template <typename T, typename... Args>
// 	    requires std::is_enum_v<T>
// 	Error(T error, Args &&...args) : code(stringFromEnum(error)) {
// 		std::ostringstream stream;
// 		(stream << ... << std::forward<Args>(args));
// 		context = std::move(stream).str();
// 	}
// };

struct Error {
	std::string code;
	std::string context;
	std::source_location location;

	template <typename T>
	    requires std::is_enum_v<T>
	Error(T error, std::string context, std::source_location location = std::source_location::current())
	    : code(stringFromEnum(error)), context(std::move(context)), location(location) {}
};

inline void printError(const Error &error) {
	std::println("{}:{}:{}: Error: {}: {}", error.location.file_name(), error.location.line(), error.location.column(), error.code,
	             error.context);
}

} // namespace Chaos
