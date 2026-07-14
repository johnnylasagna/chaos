#include "chaos/vm/display.hpp"

namespace Chaos {

Display::Display(Memory &video_) : video(video_) {
	InitWindow(VideoWidth, VideoHeight, "Chaos");
	SetTargetFPS(60);

	buffer.resize(VideoSize);

	for (size_t i = 0; i < VideoSize; i++) {
		// Ignore std::unexpected check because it is guaranteed to be within limits
		auto result = video.read<uint8_t>(i);

		if (!result) {
			printError(result.error());
			return;
		}

		buffer[i] = result.value();
	}

	Image image{
	    .data = buffer.data(), .width = VideoWidth, .height = VideoHeight, .mipmaps = 1, .format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE};

	texture = LoadTextureFromImage(image);
}

void Display::show() {
	UpdateTexture(texture, buffer.data());

	BeginDrawing();
	DrawTexture(texture, 0, 0, WHITE);
	EndDrawing();
}

void Display::close() { CloseWindow(); }

} // namespace Chaos