#include "chaos/formatter/writer.hpp"

#include <fstream>

namespace Chaos {

std::expected<void, Error> Writer::clear(const std::string &path, uint64_t address, size_t size) {
    std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);

    if (!file.is_open()) {
        return std::unexpected(Error{WriterError::FileNotFound, path});
    }

    file.seekp(static_cast<std::streamoff>(address));

    for (size_t i = 0; i < size; ++i) {
        file.put('\0');
    }

    if (!file) {
        return std::unexpected(Error{WriterError::WriteError, path});
    }

    return {};
}

std::expected<void, Error> Writer::write(const std::string &path, uint64_t address, const std::string &binaryPath) {
	std::ifstream file(binaryPath, std::ios::binary);

	if (!file.is_open()) {
		return std::unexpected(Error{WriterError::FileNotFound, binaryPath});
	}

	std::fstream outputFile(path, std::ios::binary | std::ios::in | std::ios::out);

	if (!outputFile.is_open()) {
		return std::unexpected(Error{WriterError::FileNotFound, path});
	}

	outputFile.seekp(static_cast<std::streamoff>(address));

	char byte{};
	while (file.get(byte)) {
		outputFile.put(byte);
	}

	if (!file.eof()) {
		return std::unexpected(Error{WriterError::ReadError, binaryPath});
	}

	if (!outputFile) {
		return std::unexpected(Error{WriterError::WriteError, path});
	}

	return {};
}

}