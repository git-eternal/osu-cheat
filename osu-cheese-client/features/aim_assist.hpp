#pragma once

#include <Windows.h>
#include "../sdk/osu_vec.hpp"

namespace features
{
	class aim_assist
	{
		enum class method_e : int
		{
			LINEAR,
			DIRECTIONAL_CURVE
		};

		enum class mdc_mpoint_method_e : int
		{
			HO_TO_PDIR,
			PDIR_TO_HO,
			DYNAMIC
		};

		// Generic settings
		inline static method_e method = method_e::LINEAR;
		inline static bool  enable          = false;
		inline static bool  silent          = false;
		inline static float fov             = 0.f;
		inline static float dir_fov         = 20.f;
		inline static float safezone        = 20.f;
		inline static float scaleassist     = 1.f;
		inline static float timeoffsetratio = 0.8f;

		// Visuals settings
		inline static bool vis_fov         = false;
		inline static bool vis_safezonefov = false;

		// Directional curve settings
		inline static float               mdc_ho_ratio       = 0.5f;
		inline static float               mdc_pdir_ratio     = 0.5f; // player direction
		inline static float               mdc_midpoint_ratio = 0.1f;
		inline static mdc_mpoint_method_e mdc_method         = mdc_mpoint_method_e::PDIR_TO_HO;

		// Internal calculations and tracking
		inline static sdk::vec2 last_tick_point  {};
		inline static sdk::vec2 player_direction {};
		inline static float     velocity { 0.f };

	public:
		aim_assist() = delete;
		static auto on_tab_render() -> void;
		static auto on_wndproc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, void * reserved) -> bool;
		static auto on_render() -> void;
		static auto on_osu_set_raw_coords(sdk::vec2 * raw_coords) -> void;
		static auto osu_set_field_coords_rebuilt(sdk::vec2 * out_coords) -> void;

	private:
		static auto run_aim_assist() -> void;
	};
}