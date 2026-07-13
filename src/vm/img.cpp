#include "chaos/vm/img.hpp"

#include <fstream>

namespace Chaos {

std::expected<void, Error> Img::load(Disk &disk, const std::string &path) {
	std::ifstream file(path, std::ios::binary);

	if (!file.is_open()) {
		return std::unexpected(Error{ImgError::FileNotFound, path});
	}

	std::uint64_t address = 0;
	char byte{};

	while (file.get(byte)) {
		auto result = disk.write<std::uint8_t>(address++, static_cast<std::uint8_t>(static_cast<unsigned char>(byte)));

		if (!result) {
			return std::unexpected(result.error());
		}
	}

	if (!file.eof()) {
		return std::unexpected(Error{ImgError::ReadError, path});
	}

	return {};
}

std::expected<void, Error> Img::save(Disk &disk, const std::string &path) {
	std::ofstream file(path, std::ios::binary);

	if (!file.is_open()) {
		return std::unexpected(Error{ImgError::FileNotFound, path});
	}

	for (std::uint64_t address = 0; address < DiskSize; ++address) {
		auto result = disk.read<std::uint8_t>(address);

		if (!result) {
			return std::unexpected(result.error());
		}

		file.put(static_cast<char>(*result));
	}

	if (!file) {
		return std::unexpected(Error{ImgError::WriteError, path});
	}

	return {};
}

} // namespace Chaos