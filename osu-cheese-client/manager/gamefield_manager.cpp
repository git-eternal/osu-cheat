#include "gamefield_manager.hpp"

#include <math.h>
#include <sed/macro.hpp>

auto manager::game_field::resize(int vw, int vh, float size) -> void
{
	DEBUG_PRINTF("\n[D] view resize: %d, %d", vw, vh);

	float window_ratio = static_cast<float>(vh) / osu_native_h;

	float w = osu_field_w * window_ratio * size;
	float h = osu_field_h * window_ratio * size;

	float offset = -16.f * window_ratio;

	game_field::offset_x = (static_cast<float>(vw) - w) / 2.f;
	game_field::offset_y = (static_cast<float>(vh) - h) / 4.f * 3.f + offset;

	game_field::field_ratio = h / osu_field_h;
}

auto manager::game_field::v2f(int x, int y) -> std::pair<float, float>
{
	return std::make_pair(
		(static_cast<float>(x) - game_field::offset_x) / field_ratio,
		(static_cast<float>(y) - game_field::offset_y) / field_ratio
	);
}

auto manager::game_field::f2v(float x, float y) -> std::pair<int, int>
{
	return std::make_pair(
		static_cast<int>(x * field_ratio + game_field::offset_x),
		static_cast<int>(y * field_ratio + game_field::offset_y)
	);
}

