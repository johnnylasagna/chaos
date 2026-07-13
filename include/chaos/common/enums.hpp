#pragma once

#include <meta>
#include <cstdint>
#include <optional>
#include <string>
#include <format>

namespace Chaos {

template <typename T>
    requires std::is_enum_v<T>
constexpr std::optional<T> enumFromString(std::string_view name) {
	constexpr auto info = ^^T;

	static constexpr auto enums = std::define_static_array(std::meta::enumerators_of(info));

	template for (constexpr auto e : enums) {
		if (name == std::meta::identifier_of(e))
			return [:e:];
	}

	return std::nullopt;
}

template <typename T>
    requires std::is_enum_v<T>
std::string stringFromEnum(T value) {
	constexpr auto info = ^^T;

	static constexpr auto enums = std::define_static_array(std::meta::enumerators_of(info));

	template for (constexpr auto enumerator : enums) {
		if (value == [:enumerator:]) {
			return std::format("{}::{}", std::meta::identifier_of(info), std::meta::identifier_of(enumerator));
		}
	}

	return "";
}

} // namespace Chaos
