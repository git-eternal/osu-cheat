#pragma once

#include <tuple>
#include "../sdk/osu_vec.hpp"

namespace manager
{
	class game_field
	{
		static constexpr float osu_native_w = 640.f;
		static constexpr float osu_native_h = 480.f;
		static constexpr float osu_field_w = osu_native_w * 0.8f;
		static constexpr float osu_field_h = osu_native_h * 0.8f;
		static constexpr float osu_margin_ratio = 0.1f;
		static constexpr float osu_scale_ratio = 0.28f;

	public:
		inline static float field_ratio = 0.f;
		inline static float offset_x    = 0.f;
		inline static float offset_y    = 0.f;

		game_field() = delete;

		static auto resize(int vw, int vh, float size = 1.f) -> void;
		
		// view to field (/world) | For abstraction purposes: LEGACY! DO NOT USE!!!
		[[deprecated("Use vec2")]] static auto v2f(int x, int y) -> std::pair<float, float>;

		// Field to view | For abstraction purposes: LEGACY! DO NOT USE!!!
		[[deprecated("Use vec2")]] static auto f2v(float x, float y) -> std::pair<int, int>;
	};
}