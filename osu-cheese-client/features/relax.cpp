#include "relax.hpp"

#include <imgui.h>

#include <sed/macro.hpp>
#include "../sdk/osu_vec.hpp"
#include "../game.hpp"

auto features::relax::on_tab_render() -> void
{
	if (!ImGui::BeginTabItem("Relax"))
		return;

	ImGui::Checkbox("Relax", &enable);
	OC_IMGUI_HOVER_TXT("Auto click hit objects.");
	ImGui::SliderFloat("Edge field", &edge, 0.f, 300.f);
	OC_IMGUI_HOVER_TXT("Edge field area where relax is enabled. (0 = Global)");
	ImGui::SliderInt("Timing offset", &offset, -10000, 10000);
	OC_IMGUI_HOVER_TXT("Hit timing offset from the hit object's time.");
	ImGui::Checkbox("Randomize timing offset", &offsetrand);
	OC_IMGUI_HOVER_TXT("Randomizes the timing offset and uses the current timing offset as the range.");

	ImGui::EndTabItem();
}

auto features::relax::on_wndproc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, void * reserved) -> bool
{
	static const void * filter_ho = nullptr;

	if (Msg != WM_MOUSEMOVE || !enable || !game::pp_phitobject || !game::pp_info_player->async_complete || game::pp_info_player->is_replay_mode || !game::p_game_info->is_playing)
		return false;

	auto [ho, i] = game::pp_phitobject.get_coming_hitobject(game::p_game_info->beat_time);

	if (!ho)
		return false;

	if (offset >= 0)
		--ho;

	if (ho->time.start + offset <= game::p_game_info->beat_time || ho == filter_ho)
		return false;

	filter_ho = ho;

	INPUT inp[2] { 0 };

	inp[0].type = INPUT_KEYBOARD;
	inp[0].ki.wVk = game::pp_pplayer_keys->ppk->osu.left_click.vkey;

	inp[1].type = INPUT_KEYBOARD;
	inp[1].ki.dwFlags = KEYEVENTF_KEYUP;
	inp[1].ki.wVk = game::pp_pplayer_keys->ppk->osu.left_click.vkey;

	SendInput(2, inp, sizeof(INPUT));

	return false;
}

auto features::relax::on_render() -> void 
{}

auto features::relax::on_osu_set_raw_coords(sdk::vec2 * raw_coords) -> void
{}

auto features::relax::osu_set_field_coords_rebuilt(sdk::vec2 * out_coords) -> void 
{}
