#pragma once

#include <Windows.h>
#include "sdk/osu_vec.hpp"

namespace oc
{
	class menu
	{
	public:
		menu() = delete;

		inline static bool      visible = false;
		inline static sdk::vec2 freeze_view_point; // Used for locking the player position when the menu is open

		static auto render() -> void;
		static auto wndproc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) -> bool;
	};
}