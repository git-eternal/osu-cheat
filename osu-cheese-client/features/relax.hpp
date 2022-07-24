#pragma once

#include <Windows.h>
#include "../sdk/osu_vec.hpp"

namespace features
{
	class relax
	{
		inline static bool  enable = false;
		inline static float edge = 80.f;
		inline static int   offset = -50;
		inline static bool  offsetrand = false;

	public:
		relax() = delete;
		static auto on_tab_render() -> void;
		static auto on_wndproc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, void * reserved) -> bool;
		static auto on_render() -> void;
		static auto on_osu_set_raw_coords(sdk::vec2 * raw_coords) -> void;
		static auto osu_set_field_coords_rebuilt(sdk::vec2 * out_coords) -> void;
	};
}