#include "chaos/hex/viewer.hpp"

#include <fstream>
#include <print>

#include "chaos/common/instructions.hpp"

namespace Chaos {

std::expected<void, Error> Viewer::open(const std::string &path) {
	std::ifstream file(path, std::ios::binary);

	if (!file) {
		return std::unexpected(Error{ViewerError::FileOpenFailed, path});
	}

	file.seekg(0, std::ios::end);
	auto size = file.tellg();

	if (size < 0) {
		return std::unexpected(Error{ViewerError::FileReadFailed, path});
	}

	file.seekg(0, std::ios::beg);

	bytes.resize(static_cast<size_t>(size));

	if (!file.read(bytes.data(), size)) {
		return std::unexpected(Error{ViewerError::FileReadFailed, path});
	}

	return {};
}

ftxui::Element Viewer::render(size_t address, size_t rows) const {
	using namespace ftxui;

	Elements lines;

	for (size_t row = 0; row < rows; ++row) {
		const size_t offset = address + row * 8;

		if (offset >= bytes.size())
			break;

		Elements parts;

		parts.push_back(text(std::format("{:08X}", offset)) | color(Color::GrayDark));

		parts.push_back(text(" │ "));

		std::string hex;

		for (size_t col = 0; col < 8; ++col) {
			if (offset + col < bytes.size()) {
				std::format_to(std::back_inserter(hex), "{:02X} ", static_cast<unsigned>(static_cast<unsigned char>(bytes[offset + col])));
			} else {
				hex += "   ";
			}
		}

		parts.push_back(text(hex) | color(Color::White));

		parts.push_back(text("│ "));

		std::string ascii;

		for (size_t col = 0; col < 8 && offset + col < bytes.size(); ++col) {
			unsigned char c = static_cast<unsigned char>(bytes[offset + col]);

			ascii += std::isprint(c) ? static_cast<char>(c) : '.';
		}

		parts.push_back(text(std::format("{:<8}", ascii)) | color(Color::CyanLight));

		if (offset + 8 <= bytes.size()) {

			auto opcode = static_cast<Opcode>(static_cast<unsigned char>(bytes[offset + 0]));
			auto rd = static_cast<Register>(static_cast<unsigned char>(bytes[offset + 1]));
			auto rs1 = static_cast<Register>(static_cast<unsigned char>(bytes[offset + 2]));
			auto rs2 = static_cast<Register>(static_cast<unsigned char>(bytes[offset + 3]));

			uint32_t imm = (uint32_t(uint8_t(bytes[offset + 4])) << 24) | (uint32_t(uint8_t(bytes[offset + 5])) << 16) |
			               (uint32_t(uint8_t(bytes[offset + 6])) << 8) | uint32_t(uint8_t(bytes[offset + 7]));

			parts.push_back(text(" │ "));
			parts.push_back(text(std::format("{:<10}", shortEnumName(opcode))) | bold | color(Color::GreenLight));
			parts.push_back(text(std::format("{:<4}", shortEnumName(rd))) | color(Color::YellowLight));
			parts.push_back(text(std::format("{:<4}", shortEnumName(rs1))) | color(Color::Yellow));
			parts.push_back(text(std::format("{:<4}", shortEnumName(rs2))) | color(Color::Yellow));
			parts.push_back(text(std::format("{:#010X}", imm)) | color(Color::MagentaLight));
		}

		lines.push_back(hbox(std::move(parts)));
	}

	return vbox(std::move(lines));
}

} // namespace Chaos