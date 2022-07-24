#include "menu.hpp"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <sed/macro.hpp>
#include <sed/windows/smart_handle.hpp>

#include "game.hpp"
#include "features/features.hpp"
#include "oc_client.hpp"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

auto oc::menu::render() -> void
{
	if (!menu::visible)
		return;
	
	ImGui::SetNextWindowSize(ImVec2(660.f, 460.f), ImGuiCond_FirstUseEver);

	ImGui::Begin("osu!");

	if (ImGui::Button("Unload"))
		sed::smart_handle(CreateThread(nullptr, NULL, [](LPVOID arg) -> DWORD { oc::unload(); return 0; }, nullptr, NULL, nullptr));

	ImGui::SameLine();
	ImGui::Text("[PAUSE] Key to toggle menu.");

	if (ImGui::BeginTabBar("##oc_tabs"))
	{
		features::dispatcher::on_tab_render();
		ImGui::EndTabBar();	
	}
	ImGui::End();
}

auto oc::menu::wndproc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) -> bool
{
	if (Msg == WM_KEYDOWN && wParam == VK_PAUSE)
	{
		menu::visible = !menu::visible;
		ShowCursor(menu::visible);

		if (menu::visible)
			menu::freeze_view_point = game::pp_viewpos_info->pos;

		if (menu::visible && game::pp_raw_mode_info->is_raw || !menu::visible)
		{
			POINT p = menu::freeze_view_point;
			ClientToScreen(game::hwnd, &p);
			SetCursorPos(p.x, p.y);
		}

		return true;
	}
	
	if (!menu::visible)
		return false;
	
	ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);

	switch (Msg)
	{
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_XBUTTONDBLCLK:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
			return true;
	}

	return false;
}