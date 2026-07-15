#include "chaos/vm/display.hpp"

namespace Chaos {

Display::Display(Memory &video_) : video(video_) {
	SetTraceLogLevel(LOG_NONE);
	InitWindow(VideoWidth, VideoHeight, "Chaos");
	SetTargetFPS(60);

	buffer.resize(VideoSize);

	auto result = video.copy(buffer);

	if (!result) {
		printError(result.error());
		return;
	}

	Image image{
	    .data = buffer.data(), .width = VideoWidth, .height = VideoHeight, .mipmaps = 1, .format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE};

	texture = LoadTextureFromImage(image);
}

void Display::show() {
	if (video.dirty.exchange(false)) {
		auto result = video.copy(buffer);
		if (!result) {
			printError(result.error());
			return;
		}
		UpdateTexture(texture, buffer.data());
	}

	BeginDrawing();
	DrawTexture(texture, 0, 0, WHITE);
	EndDrawing();
}

Display::~Display() {
	UnloadTexture(texture);
	CloseWindow();
}

} // namespace Chaos